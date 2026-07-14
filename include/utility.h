#ifndef UTLILTY_H
#define UTLILTY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "global.h"

//char -> 16진수 변환
#define HEX_TO_NUM(c) ((c) >= 'a' ? (c) - 'a' + 10 : \
                    (c) >= 'A' ? (c) - 'A' + 10 : \
                    (c) - '0')
                    

//현재 날짜 및 시간 출력
void get_current_time(char* outbuf, size_t buflen);
void get_current_date(char* outbuf, size_t buflen);

//시리얼 포트 자동 탐색 (VID기반)
int find_canable_port();

void ringbuf_init(RingBuffer** buf);
int ringbuf_isempty(RingBuffer* buf);
int ringbuf_isfull(RingBuffer* buf);
void ringbuf_register_data(RingBuffer* buf, CANFrame* frame);
void ringbuf_get_data(RingBuffer* buf, CANFrame* frame);
void ringbuf_clear(RingBuffer** buf);

void list_init(ListNode** head, ListNode** tail);
uint8_t list_isempty(ListNode** head, ListNode** tail);
uint8_t list_searchdata(ListNode** head, int id);
void list_addnode(ListNode** list, ListNode** tail, int id);
void list_deletenode(ListNode** list, ListNode** tail, int id);
void list_clear(ListNode** head, ListNode** tail);

#endif
