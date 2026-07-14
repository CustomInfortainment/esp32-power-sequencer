#include "file.h"

//파일 저장 위치
const char FILE_SAVE_PATH[256];
static char DIR_NAME[256];

//생성한 파일 정보 배열 접급 인덱스
static int IDS_INDEX = 0;
static IDFile idfiles[MAX_IDS];

//임시 함수
static uint64_t elapsed_sum;
static uint64_t elapsed_cnt;

static int check_dir(char* path)
{
    struct stat st;
    char buf[256];
    snprintf(buf, sizeof(buf), "%s", path);
    return (stat(buf, &st) == 0 && S_ISDIR(st.st_mode));
}

static FILE* check_file(int id)
{
    for(int i = 0; i < MAX_IDS; i++)
    {
        if(idfiles[i].id == id)
        {
            return idfiles[i].fp;
        }
    }
    return NULL;
}

static int get_dir(char* dirname)
{
    int result = 0;
    char result_path[256];

    //현재 날짜 기준 폴더 생성
    get_current_date(DIR_NAME, sizeof(DIR_NAME));

    snprintf(result_path, sizeof(result_path), "%s/%s", SAVE_PATH, DIR_NAME);

    result = mkdir(result_path, 0755);

    if(result == 0)
    {
        snprintf(dirname, sizeof(dirname), "%s", result_path);
        return result;
    }
    return result;
}

static FILE* get_file(int id)
{
    FILE* isfile = NULL;
    int isDir = 0;

    char dir_path_buf[256];
    char file_path_buf[256];

    get_dir(dir_path_buf);

    //저장할 디렉터리 및 파일이 있는지 확인
    isDir = check_dir(dir_path_buf);
    isfile = check_file(id);

    //파일이 없다면 새로 생성
    if(isfile == NULL && IDS_INDEX < MAX_IDS)
    {
        IDFile* entry = &idfiles[IDS_INDEX++];

        entry->id = id;
        memset(file_path_buf, '\0', sizeof(file_path_buf));

        //buf에 파일 경로를 새로 씀
        snprintf(file_path_buf, sizeof(file_path_buf), "%s/%s/CAN_%#05X.txt", SAVE_PATH, DIR_NAME, id);
        snprintf(entry->filename, sizeof(entry->filename), "CAN_%#05X.txt", id);

        //파일 염
        entry->fp = fopen(file_path_buf, "a");

        return entry->fp;
    }
    else 
    {
        return isfile;
    }
    return NULL;
}

static void create_format_frame(int dlc, char* data, char* formatdata_buf)
{
    if(formatdata_buf == NULL) return;

    int idx = 0;

    for(int i = 0; i < dlc; i++)
    {
        formatdata_buf[idx] = data[i * 2];
        formatdata_buf[idx + 1] = data[i * 2 + 1];
        formatdata_buf[idx + 2] = ' ';

        idx += 3;
    }
}

void save_frame(CANFrame* frame)
{
    uint64_t now = now_ns();

    int id = frame->id;
    int dlc = frame->dlc;
    char* rawdata = frame->raw_data;

    char formatdata_buf[256];

    create_format_frame(dlc, rawdata, formatdata_buf);

    FILE* entry = get_file(id);

    if(entry == NULL) return;

    fprintf(entry, "id:0X%#03X dlc:%d data:%s\n", id, dlc, formatdata_buf);

    uint64_t end = now_ns();
    uint64_t elapsed = end - now;

    elapsed_sum += elapsed;
    elapsed_cnt++;

    metric_report_ns(&elapsed_sum, &elapsed_cnt);
}

#if 0
void file_data_save(CANFrame* frame)
{
    char format_data[40];
    FILE* fp = NULL;

    int dst_idx = 0;

    if(is_save == 0) return;
    
    for(int i = 0; i < frame->dlc; i++)
    {
        format_data[dst_idx] = frame->raw_data[i * 2];
        format_data[dst_idx + 1] = frame->raw_data[i * 2 + 1];
        format_data[dst_idx + 2] = ' ';

        dst_idx += 3;
    }

    format_data[dst_idx - 1] = '\0';

    //id마스크 구현, 마스크 리스트에 있는 id는 저장을 건너뛴다.
    if(list_searchdata(&maskIdList.head, frame->id) == 1)
    {
        return;
    }
    //id필터링 구현, 필터 마스크에 있는 id만 저장한다.
    if(list_searchdata(&filterIdList.head, frame->id) == 1 || list_isempty(&filterIdList.head, &filterIdList.tail) == 1)
    {
        fp = get_file(frame->id);
    }

    if(fp != NULL)
    {
        fprintf(fp, "id:%03X DLC:%d, data:%s\n", frame->id, frame->dlc, format_data);
    }
}
#endif

void all_exit_files()
{
    for(int i = 0; i < IDS_INDEX; i++)
    {
        if(idfiles[i].fp != NULL) fclose(idfiles[i].fp);
    }
}