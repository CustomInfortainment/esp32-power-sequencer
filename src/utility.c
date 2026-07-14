#include "utility.h"
#include "handler.h"
#include "sniffing.h"

//-------- 현재 시간 출력 ----------
void get_current_time(char* outbuf, size_t buflen)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    size_t size = strlen(outbuf);

    snprintf(outbuf, buflen, "%d:%d;%d", 
    tm.tm_hour, 
    tm.tm_min, 
    tm.tm_sec);
}

void get_current_date(char* outbuf, size_t buflen)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    size_t size = strlen(outbuf);

    snprintf(outbuf, buflen, "%d-%d-%d", 
    tm.tm_year + 1900, 
    tm.tm_mon + 1, 
    tm.tm_mday);
}

//시리얼 포트 자동 탐색 (VID기반)
int find_canable_port()
{
    
}

//--------- 링버퍼 구현부 -----------

void ringbuf_init(RingBuffer** ringbuf)
{
    char log[256];

    snprintf(log, sizeof(log), "링버퍼 초기화 중...");
    prt_log_console(log, 1);

    *ringbuf = malloc(sizeof(RingBuffer));

    if(*ringbuf != NULL)
    {
        (*ringbuf)->head = 0;
        (*ringbuf)->tail = 0;
        memset((*ringbuf)->buf, 0, sizeof((*ringbuf)->buf));

        snprintf(log, sizeof(log), "링버퍼 초기화 완료");
        prt_log_console(log, 1);
    }
}

int ringbuf_isempty(RingBuffer* ringbuf)
{
    if(ringbuf->head == ringbuf->tail)
    {
        return 1;
    }
    return 0;
}

int ringbuf_isfull(RingBuffer* ringbuf)
{
    //한 칸 남았으면 그냥 꽉 찬걸로 인지함.
    if(((ringbuf->head + 1) % RING_BUF_SIZE) == ringbuf->tail)
    {
        return 1;
    }
    return 0;
}

//버퍼에 저장
void ringbuf_register_data(RingBuffer* ringbuf, CANFrame* frame)
{
    if(ringbuf_isfull(ringbuf) == 1) //오버플로우 상태라면
    {
        return;
    }
    memcpy(&ringbuf->buf[ringbuf->head], frame, sizeof(CANFrame));
    ringbuf->head = (ringbuf->head + 1) % RING_BUF_SIZE;
}

//버퍼 -> outframe
void ringbuf_get_data(RingBuffer* ringbuf, CANFrame* outframe)
{
    if(ringbuf_isempty(ringbuf) == 1) //언더플로우 상태라면
    {
        return;
    }
    int idx = ringbuf->tail;
    ringbuf->tail = (ringbuf->tail + 1) % RING_BUF_SIZE;

    memcpy(outframe, &ringbuf->buf[idx], sizeof(CANFrame));
}

void ringbuf_clear(RingBuffer** ringbuf)
{
    prt_log_console("버퍼를 초기화 합니다...", 1);
    //버퍼가 비어져 있다면
    if(ringbuf_isempty(*ringbuf) == 1)
    {
        *ringbuf = NULL;
        return;
    }
    
    for(int i = (*ringbuf)->head; i < (*ringbuf)->tail; i++)
    {
        (*ringbuf)->buf[i] = NULL;
    }
}

//--------- 이중 연결 리스트 구현부 -----------

void list_init(ListNode** head, ListNode** tail)
{
    *head = NULL;
    *tail = NULL;
}

uint8_t list_isempty(ListNode** head, ListNode** tail)
{
    if(*head == NULL && *tail == NULL)
    {
        prt_log_console("빈 리스트 탐지", 1);
        return 1;
    }
    return 0;
}
uint8_t list_searchdata(ListNode** head, int id)
{
    ListNode *curnode = *head;

    while (curnode != NULL)
    {
        if(curnode->id == id)
        {
            return 1;
        }
        curnode = curnode->next;
    }
    return 0;
}

void list_addnode(ListNode** head, ListNode** tail, int id)
{
    ListNode *node = (ListNode*)malloc(sizeof(ListNode));

    if(node == NULL)
        return;

    node->id = id;
    node->prev = NULL;
    node->next = NULL;
    
    if(*head == NULL)
    {
        *tail = node;
        *head = node;
    }
    else
    {
        node->prev = *tail;
        (*tail)->next = node;
        *tail = node;
    }
}

void list_deletenode(ListNode** head, ListNode** tail, int id)
{
    ListNode *curnode;
    curnode = *head;

    //노드가 한개 뿐일 때
    if((*head) == (*tail))
    {
        if((*head)->id == id)
        {
            free(*head);
            *head = NULL;
            *tail = NULL;

            return;
        }
    }

    if((*head)->id == id)
    {
        ListNode *node = *head;
        *head = node->next;
        (*head)->prev = NULL;
        free(node);

        return;
    }

    while(curnode != NULL)
    {
        if(curnode->id == id)
        {
            //지우고자 하는 노드가 마지막 노드일때
            if(curnode->next == NULL)
            {
                *tail = curnode->prev;
                (*tail)->next = NULL;
                free(curnode);
                return;
            }
            ListNode *prevnode = curnode->prev;
            ListNode *nextnode = curnode->next;
            prevnode->next = curnode->next;
            nextnode->prev = prevnode;
            free(curnode);
            return;
        }
        curnode = curnode->next;
    }
}

void list_clear(ListNode** head, ListNode** tail)
{
    prt_log_console("리스트를 초기화 합니다...", 1);
    ListNode *curNode;
    ListNode *nextNode;
    curNode = *head;

    while(curNode != NULL)
    {
        nextNode = curNode->next;
        free(curNode);
        curNode = nextNode;
    }
    *head = NULL;
    *tail = NULL;
}