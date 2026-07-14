#ifndef FILE_H
#define FILE_H

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "global.h"

#define MAX_IDS 2048 //최대 로그 파일 갯수
#define SAVE_PATH "/Users/choemingyu/Infortainment/esp32-power-sequencer/host/sniff/data"

extern const char FILE_SAVE_PATH[256];

int save_frame(CANFrame* frame);

void all_exit_files();

#endif