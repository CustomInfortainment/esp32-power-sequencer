#include "sniffing.h"
#include "log.h"
#include "file.h"
#include "parser.h"
#include "utility.h"

int id_count = 0;
int serial_fd = 0;

RingBuffer* canframeRingBuf;

static ProfilingFrame canProfiling;

char id_str[4];

void sniff_init()
{
    serial_fd = open(SERIAL_PORT_PATH, O_RDWR | O_NOCTTY);

    if(serial_fd < 0)
    {
        perror("시리얼 포트 열기 실패");
        return;
    }

    struct termios tty;
    tcgetattr(serial_fd, &tty);
    cfsetspeed(&tty, B115200);
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~CRTSCTS;
    tty.c_cflag |= CREAD | CLOCAL;
    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO;
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_oflag &= ~OPOST;
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 1;
    tcsetattr(serial_fd, TCSANOW, &tty);

    write(serial_fd, "C\r", 2);
    usleep(500000);
    write(serial_fd, "S6\r", 3);
    usleep(500000);
    write(serial_fd, "O\r", 2);
    usleep(500000);

    //링버퍼 초기화
    ringbuf_init(&canframeRingBuf);

    write(serial_fd, "C\r", 2);
    usleep(500000);
    write(serial_fd, "S6\r", 3);
    usleep(500000);
    write(serial_fd, "O\r", 2);
    usleep(500000);
}

void sniff_stop()
{
    write(serial_fd, "C\r", 2);
    prt_log_console("CAN 프레임 수신을 중지합니다.", 1);
}

void sniff_data_recv()
{
    char buf[256];
    char raw_frame[256];

    int raw_frame_size = 0;

    double fps;

    CANFrame frame;

    // 256byte 만큼 읽고
    int read_bytes = read(serial_fd, buf, sizeof(buf));

    if (read_bytes <= 0) {
        prt_log_console("데이터 로드 대기 중...", 1);
        usleep(200000);
        return;
    }
    
    // 1바이트씩 불러와서 읽음.
    for(int b = 0; b < read_bytes; b++)
    {
        char current_byte = buf[b];

        if (raw_frame_size >= 255) 
        {
            raw_frame_size = 0; 
        }

        if(raw_frame_size == 0 && current_byte != 't')
        {
            raw_frame[0] = '\0';
            raw_frame_size = 0;
            continue;
        }
        raw_frame[raw_frame_size++] = current_byte;

        // 표준 CAN 데이터만 거름. 소문자가 't', frame 배열 완성.
        if(raw_frame[0] == 't' && raw_frame[raw_frame_size - 1] == '\r') 
        {
            memset(&frame, 0, sizeof(CANFrame));
            
            id_str[0] = raw_frame[1];
            id_str[1] = raw_frame[2];
            id_str[2] = raw_frame[3];
            id_str[3] = '\0';

            frame.id = 
            HEX_TO_NUM(raw_frame[1]) << 8 | 
            HEX_TO_NUM(raw_frame[2]) << 4 | 
            HEX_TO_NUM(raw_frame[3]);

            frame.dlc = raw_frame[4] - '0';
            
            memcpy(frame.raw_data, raw_frame + 5, frame.dlc * 2);

            //버퍼에 저장
            ringbuf_register_data(canframeRingBuf, &frame);

            canProfiling.g_frame_cnt++;

            //파싱한 데이터 출력
            prt_parsing_to_console(frame.id, frame.raw_data);

            raw_frame_size = 0;
            raw_frame[0] = '\0';
        }
    }

    fps = metric_report_fps(&canProfiling);

    if(fps != 0)
    {
        fprintf(stderr, "프레임 수신 시간까지 FPS : %lf\n", fps);
        fprintf(stderr,"프레임 수신 까지 걸린 시간 : %lf\n", (1 / fps) * 1000000000ULL);
    }

    while(ringbuf_isempty(canframeRingBuf) == 0)
    {
        CANFrame frame_buf;

        ringbuf_get_data(canframeRingBuf, &frame_buf);
        save_frame(&frame_buf);
    }
}