#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>

#include "file.h"
#include "sniffing.h"
#include "utility.h"
#include "parser.h"
#include "canid.h"
#include "terminal.h"

#define MAX_LOG_LENGTH 256

void can_id_str(int id, char* outlog);
void can_dlc_str(int dlc, char* buf);

void prt_log_console(char* log, int isnewline);
void prt_log_console_handler(int id, int dlc, char* formatdata);
void prt_parsing_to_console(int id, char* data);

#endif