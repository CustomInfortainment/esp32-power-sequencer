#ifndef HANDLER_H
#define HANDLER_H

#include <stdio.h>
#include "terminal.h"
#include "log.h"
#include "global.h"
#include "sniffing.h"
#include "file.h"

//--------시스템 콜 핸들러 구현부---------
void handler_sigint(int sig);

//--------CLI모드 핸들러 구현부---------
char handler_inputch();
void handler_cmd_mode(CMDFrame** frame);
void handler_running_mode();

//--------일반 CLI 명령 핸들러 구현부---------
void handler_cmd_filter(CMDFrame* frame);
void handler_cmd_unfilter(CMDFrame* frame);
void handler_cmd_mask(CMDFrame* frame);
void handler_cmd_unmask(CMDFrame* frame);

#if 0
void handler_cmd_setpath(CMDFrame* frame, ENVFrame* envframe);
void handler_cmd_setmodule(CMDFrame* frame, ENVFrame* envframe);
#endif

//--------일반 CLI OPT 명령 핸들러 구현부---------
void handler_opt_save(CMDFrame* frame);
void handler_opt_prt(CMDFrame* frame);

#endif