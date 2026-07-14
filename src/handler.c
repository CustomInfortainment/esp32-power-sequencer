#include "handler.h"

//--------시스템 콜 핸들러 구현부---------

void handler_sigint(int sig)
{
    if(filterIdList.head != NULL && filterIdList.tail != NULL)
        list_clear(&filterIdList.head, &filterIdList.tail);
    if(canframeRingBuf != NULL)
        ringbuf_clear(&canframeRingBuf);

    exit(0);
}

//--------CLI모드 핸들러 구현부---------

//한글자 단위로 명령 입력받음
char handler_inputch()
{
    char output;
    if((output = (char)fgetc(stdin)) == EOF)
    {
        prt_log_console("입력 오류 발생", 1);
        return '\0';
    }
    return output;
}

//명령 단위로 입력 받음
void handler_cmd_mode(CMDFrame** frame)
{
    printf("sniffer > ");

    *frame = (CMDFrame*)malloc(sizeof(CMDFrame));

    char buf[MAX_CMD_LINE];
    int argv = 0;

    int in_token = 0;

    if(*frame == NULL)
	{
		prt_log_console("메모리 미할당 오류", 1);
		return;
	}
	if(fgets(buf, MAX_CMD_LINE, stdin) == NULL)
	{
        prt_log_console("입력 오류", 1);
		return;
	}

    //토큰 기준으로 명령어 갯수 구함
    for(int i = 0; i < strlen(buf); i++)
    {
        if(buf[i] == CMD_TOKEN)
        {
            in_token = 1;
            argv++;
        }
        else if(in_token == 1)
        {
            in_token = 0;
        }
    }
    
    (*frame)->argv = ++argv;

    if((*frame)->argv != 0)
    {
        (*frame)->args = (char**)malloc(sizeof(char*) * argv);

        for(int i = 0; i < argv; i++)
        {
            if(i != 0)
                (*frame)->args[i] = strtok(NULL, " ");
            else
                (*frame)->args[i] = strtok(buf, " ");
            //끝에 개행문자는 지운다.
            (*frame)->args[i][strcspn((*frame)->args[i], "\n")] = '\0';
        }

        //첫번쨰 인덱스는 명령 자체이므로 1부터 시작
        for(int i = 1; i < argv; i++)
        {
            //-를 제외한 opt값 저장
            if((*frame)->args[i][0] != '-')
            {
                break;
            }
            (*frame)->opts[i - 1] = (*frame)->args[i][1];
        }
        
        if((*frame)->args != NULL && (*frame)->args[0][0] != '\0')
        {
            if(strcmp((*frame)->args[0], CMD_FILTER) == 0 || strcmp((*frame)->args[0], CMD_MASK) == 0
        || strcmp((*frame)->args[0], CMD_UNFILTER) == 0 || strcmp((*frame)->args[0], CMD_UNMASK) == 0)
            {
                //마지막 인자가 16진수일 때만 유효한 커맨드로 인정.
                if((*frame)->args[(*frame)->argv - 1][0] == '0')
                {
                    if((*frame)->args[(*frame)->argv - 1][1] == 'X' || (*frame)->args[(*frame)->argv - 1][1] == 'x')
                    {
                        char* hexdata = (*frame)->args[(*frame)->argv - 1];

                        if(hexdata[0] == '0' && (hexdata[1] == 'X' || hexdata[1] == 'x'))
                        {
                            hexdata += 2;

                            char log[256];
                            snprintf(log, sizeof(log), "2:%s", hexdata);
                            prt_log_console(log, 1);
                        }
                        
                        size_t hexsize = strlen(hexdata);

                        for(int i = hexsize - 1; i >= 0; i--)
                        {
                            (*frame)->canid += (HEX_TO_NUM(hexdata[(hexsize - 1) - i])) << (i * 4);
                        }
                    }
                    else
                    {
                        prt_log_console("16진수 입력 시 0x 혹은 0X를 포함시켜 주세요.", 1);
                        free(*frame);
                        return;
                    }
                }
                if(strcmp((*frame)->args[0], CMD_FILTER) == 0)
                {
                    handler_cmd_filter(*frame);
                    free(*frame);
                }
                else if(strcmp((*frame)->args[0], CMD_MASK) == 0)
                {
                    handler_cmd_mask(*frame);
                    free(*frame);
                }
                else if(strcmp((*frame)->args[0], CMD_UNFILTER) == 0)
                {
                    handler_cmd_unfilter(*frame);
                    free(*frame);
                }
                else if(strcmp((*frame)->args[0], CMD_UNMASK) == 0)
                {
                    handler_cmd_unmask(*frame);
                    free(*frame);
                }
                return;
            }
            else if(strcmp((*frame)->args[0], CMD_EXIT) == 0)
            {
                free(*frame);
                prt_log_console("프로세스를 종료합니다..", 1);
                raise(SIGINT);
            }
            else if(strcmp((*frame)->args[0], CMD_RUN) == 0)
            {
                for(int i = 0; i < strlen((*frame)->opts); i++)
                {
                    if((*frame)->opts[i] == OPT_PRINT)
                    {
                        handler_opt_prt(*frame);
                    }
                    else if((*frame)->opts[i] == OPT_SAVE)
                    {
                        handler_opt_save(*frame);
                    }
                }
                free(*frame);
                set_cli_mode(RUNNING);
                prt_log_console("RUNNING 모드로 전환", 1);
            }
            else
            {
                prt_log_console("명령을 다시 입력하십시오.", 1);
                free(*frame);
                return;
            }
        }
    }
    else
    {
        free(*frame);
        return;
    }
}

void handler_running_mode()
{
    char cmd;
	int n = read(STDIN_FILENO, &cmd, 1);

	//커맨드 모드로 전환 및 따옴표 입력 받으면 전환
	if(n > 0 && cmd == CMD_EXIT_RUINNING)
    {
        sniff_stop();
		set_cli_mode(CMD);
    }
    sniff_data_recv();
}

//--------일반 CLI 명령 핸들러 구현부---------

void handler_cmd_filter(CMDFrame* frame)
{
    //해당 명령은 인자를 반드시 포함해야 함.
    if(strcmp(frame->args[0], CMD_FILTER) != 0)
    {
        prt_log_console("명령이 유효하지 않습니다. 필터링 대상 ID를 입력해주세요.", 1);
        return;
    }

    list_addnode(&filterIdList.head, &filterIdList.tail, frame->canid);
    char log[256];
    snprintf(log, sizeof(log), "필터링이 적용 되었습니다 -> 0X%03X", frame->canid);
    prt_log_console(log, 1);
}

void handler_cmd_unfilter(CMDFrame* frame)
{
    if(strcmp(frame->args[0], CMD_UNFILTER) != 0)
    {
        prt_log_console("명령이 유효하지 않습니다.", 1);
        return;
    }
    if(frame->canid != 0)
    {
        list_deletenode(&filterIdList.head, &filterIdList.tail, frame->canid);
        char log[256];
        snprintf(log, sizeof(log), "필터링이 해제 되었습니다 -> 0X%03X", frame->canid);
        prt_log_console(log, 1);
    }
    else
    {
        prt_log_console("필터링 된 모든 ID가 날라갑니다. (y/n) : ", 0);

        while(1)
        {
            char input = handler_inputch();
            int time = 0;

            if(input == 'y' || input == 'Y')
            {
                list_clear(&filterIdList.head, &filterIdList.tail);
                prt_log_console("필터링 된 모든 ID가 초기화 되었습니다.", 1);
                break;
            }
            else if(input == 'n' || input == 'N')
            {
                break;
            }
            else
            {
                time++;
                prt_log_console("다시 입력해주세요.", 1);

                if(time == 3)
                {
                    time = 0;
                    prt_log_console("최대 입력 횟수 초과", 1);
                    break;
                }
            }
        }
    }
}

void handler_cmd_mask(CMDFrame* frame)
{
    if(strcmp(frame->args[0], CMD_MASK) != 0)
    {
        prt_log_console("명령이 유효하지 않습니다.", 1);
        return;
    }

    list_addnode(&maskIdList.head, &maskIdList.tail, frame->canid);
    char log[256];
    snprintf(log, sizeof(log), "필터링이 적용 되었습니다 -> 0X%03X", frame->canid);
    prt_log_console(log, 1);
}

void handler_cmd_unmask(CMDFrame* frame)
{
    if(strcmp(frame->args[0], CMD_UNMASK) != 0)
    {
        prt_log_console("명령이 유효하지 않습니다.", 1);
        return;
    }
    if(frame->canid != 0)
    {
        list_deletenode(&maskIdList.head, &maskIdList.tail, frame->canid);
        char log[256];
        snprintf(log, sizeof(log), "필터링이 해제 되었습니다 -> 0X%03X", frame->canid);
        prt_log_console(log, 1);
    }
    else
    {
        prt_log_console("필터링 된 모든 ID가 날라갑니다. (y/n) : ", 1);

        while(1)
        {
            char input = handler_inputch();
            int time = 0;

            if(input == 'y' || input == 'Y')
            {
                list_clear(&maskIdList.head, &maskIdList.tail);
                prt_log_console("필터링 된 모든 ID가 초기화 되었습니다.", 1);
                break;
            }
            else if(input == 'n' || input == 'N')
            {
                break;
            }
            else
            {
                time++;
                prt_log_console("다시 입력해주세요.", 1);

                if(time == 3)
                {
                    time = 0;
                    prt_log_console("최대 입력 횟수 초과", 1);
                    break;
                }
            }
        }
    }
}

#if 0
void handler_cmd_setpath(CMDFrame* frame, ENVFrame* envframe)
{
    if(frame == NULL)
    {
        return;
    }
    if(envframe == NULL)
    {
        envframe = (ENVFrame*)malloc(sizeof(ENVFrame));
    }
    if(strcmp(frame->args[0], CMD_SETPATH) != 0)
    {
        prt_log_console("존재하지 않는 명령어 입니다.", 1);
    }

    while(1)
    {
        if(check_dir(frame->args[1]) == 0)
        {
            prt_log_console("존재하지 않는 경로입니다. 새롭게 생성하시겠습니까? (y/n) : ", 0);
            char input = handler_inputch();

            if(input == 'y' || input == 'Y')
            {
                create_date_dir(envframe);
                prt_log_console("저장 경로가 설정되었습니다.", 1);
            }
            else if(input == 'n' || input == 'N')
            {
                free(envframe);
                return;
            }
        }
        else
        {
            prt_log_console("저장 경로가 설정되었습니다.", 1);
        }
    }
    free(envframe);
}

void handler_cmd_setmodule(CMDFrame* frame, ENVFrame* envframe)
{
    if(frame == NULL)
    {
        return;
    }
    if(envframe == NULL)
    {
        envframe = (ENVFrame*)malloc(sizeof(ENVFrame));
    }
    if(strcmp(frame->args[0], CMD_SETMODULE) != 0)
    {
        prt_log_console("존재하지 않는 명령어 입니다.", 1);
    }

    
    free(envframe);
}
#endif

//--------일반 CLI OPT 명령 핸들러 구현부---------

void handler_opt_save(CMDFrame* frame)
{
    if(frame == NULL) return;

    prt_log_console("저장", 1);

    for(int i = 0; i < strlen(frame->opts); i++)
    {
        if(frame->opts[i] == OPT_SAVE)
        {
            is_save = 1;
            return;
        }
    }
    is_save = 0;
}

void handler_opt_prt(CMDFrame* frame)
{
    if(frame == NULL) return;

    prt_log_console("출력", 1);

    for(int i = 0; i < strlen(frame->opts); i++)
    {
        if(frame->opts[i] == OPT_PRINT)
        {
            is_prt = 1;
            return;
        }
    }
    is_prt = 0;
}