#include "log.h"

//임시 함수
static uint64_t elapsed_sum;
static uint64_t elapsed_cnt;

void can_id_str(int id, char* outlog)
{
    switch(id)
    {
        case ID_ENGINE_RPM : 
            snprintf(outlog, MAX_LOG_LENGTH, "id:0x%#03X", id);
            break;
        case ID_COOLENT_TEMP:
            snprintf(outlog, MAX_LOG_LENGTH, "id:0x%#03X", id);
            break;
        default : 
            snprintf(outlog, MAX_LOG_LENGTH, "%#05X", id); //기본 id값을 16진수로 출력한다.
            break;
    }
}

void can_dlc_str(int dlc, char* outlog)
{
    snprintf(outlog, MAX_LOG_LENGTH, "%d", dlc);
}

//단순 로그 출력1
void prt_log_console(char* log, int isnewline)
{
    char date_buf[MAX_LOG_LENGTH];
    get_current_time(date_buf, sizeof(date_buf));

    if(isnewline == 0)
        printf("%s -> %s", date_buf, log);
    else
        printf("%s -> %s\n", date_buf, log);
}

//단순 로그 출력2, 실시간 파싱되지 않은 데이터 출력용.
void prt_log_console_handler(int id, int dlc, char* formatdata)
{
    char buf[MAX_LOG_LENGTH]; //최종 출력용 버퍼

    if(is_prt == 0) return;

    prt_log_console("테스트 로그 : 출력 중...", 1);

    if(list_searchdata(&maskIdList.head, id) == 1)
    {
        return;
    }
    if(list_searchdata(&filterIdList.head, id) == 1 || list_isempty(&filterIdList.head, &filterIdList.tail) == 1)
    {
        snprintf(buf, sizeof(buf), "id:%x dlc:%d data:%s\n", id, dlc, formatdata);
        prt_log_console(buf, 1);
    }
}

//파싱된 데이터 출력용, 차량 대시보드 용도.
void prt_parsing_to_console(int id, char* data)
{
    uint64_t now = now_ns();

    char buf[MAX_LOG_LENGTH];
    char date_log[MAX_LOG_LENGTH];
    char data_log[MAX_LOG_LENGTH];

    can_id_str(id, data_log);
    get_current_time(date_log, sizeof(date_log));

    snprintf(buf, MAX_LOG_LENGTH, "%s -> %s\n", date_log, data_log);

    //파싱 되지 않은 데이터는 거른다.
    if(strstr(buf, "0x") != NULL || strstr(buf, "0X") != NULL)
        return;

    static int current_rpm;
    static int current_temp;

    current_rpm = get_rpm_data(id, data);
    current_temp = get_coolent_data(id, data);

    printf("\033[2A");
    printf("\r\033[K");
    printf("rpm : %d\n", current_rpm);
    printf("\r\033[K");
    printf("coolent temp : %d\n", current_temp);

    uint64_t end = now_ns();
    uint64_t elapsed = end - now;

    elapsed_sum += elapsed;
    elapsed_cnt++;

    metric_report_ns(&elapsed_sum, &elapsed_cnt);
}
