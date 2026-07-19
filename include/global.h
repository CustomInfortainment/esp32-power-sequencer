#ifndef GLOBAL_H
#define GLOBAL_H

#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>

#define RING_BUF_SIZE 40

//명령어 모음
#define CMD_RUN "run"
#define CMD_EXIT "exit"
#define CMD_MASK "mask"
#define CMD_UNMASK "unmask"
#define CMD_FILTER "filter"
#define CMD_UNFILTER "unfilter"
#define CMD_SETPATH "setpath"
#define CMD_SETMODULE "setmodule"

#define CMD_EXIT_RUINNING 'q'

//명령어 인자 모음
#define OPT_PRINT 'p'
#define OPT_SAVE 's'

#define OPT_SHOWLIST 'l'

//명렁어 기준점
#define CMD_TOKEN ' '

//파일 저장에 쓰이는 구조체
typedef struct IDFile
{
    int id;
    FILE* fp;
    char filename[256];
} IDFile;

//CAN프레임 구조체
typedef struct CANFrame
{
    int id;
    int dlc;
    char raw_data[256];
} CANFrame;

//TUI 명령 구조체
typedef struct CMDFrame
{
    char **args;
    int argv;
    int canid;
    char opts[128];
} CMDFrame;

//자료구조 구현부-------------------
typedef struct RingBuffer
{
    CANFrame* buf[RING_BUF_SIZE];
    unsigned int head;
    unsigned int tail;
} RingBuffer;

typedef struct ListNode
{
    int id;
    struct ListNode *prev;
    struct ListNode *next;
} ListNode;

typedef struct CMDIdList
{
    ListNode* head;
    ListNode* tail;
} CMDIdList;

//코드 성능 실측 관련 구현부------------
typedef struct ProfilingFrame
{
    uint64_t last_report;
    double now_sec;
    int g_frame_cnt;
    int g_second_avg;
} ProfilingFrame;

#endif
