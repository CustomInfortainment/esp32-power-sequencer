#ifndef FILE_H
#define FILE_H

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "global.h"
#include "utility.h"
#include "log.h"
#include "profiling.h"

#define MAX_IDS 2048 //최대 로그 파일 갯수
#define SAVE_PATH "/data/data/com.termux/files/home/Sniffing_cli/data"

extern const char FILE_SAVE_PATH[256];

void save_frame(int id, int dlc, char* format_data);
void all_exit_files();

#endif
