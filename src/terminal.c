#include "terminal.h"

enum CLI_MODE currentmode;

struct termios old_settings;
struct termios new_settings;

//가장 최근에 보낸 커맨드 저장.
CMDFrame* curcmdframe = NULL;

int is_prt = 0;
int is_save = 0;

CMDIdList filterIdList;
CMDIdList maskIdList;

void set_cli_mode(enum CLI_MODE mode)
{
	if(currentmode == mode) return;
	currentmode = mode;

	if(currentmode == CMD)
	{
		restore_keypress();
	}
	else if(currentmode == RUNNING)
	{
		set_keypress();
	}
}

//논블로킹 입력 구현
void set_keypress()
{
	tcgetattr(STDIN_FILENO, &old_settings);
	tcgetattr(STDIN_FILENO, &new_settings);

	new_settings.c_lflag &= ~(ICANON | ECHO);
	new_settings.c_cc[VMIN] = 0;
	new_settings.c_cc[VTIME] = 0;

	tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);
}

//논블로킹 -> 기존 터미널 설정으로 복귀
void restore_keypress()
{
	tcsetattr(STDIN_FILENO, TCSANOW, &old_settings);
}

//반복문 안에서 계속 작동. run_terminal 참고
void run_cli_mode()
{
	switch(currentmode)
	{
		case RUNNING:
			handler_running_mode();
			break;
		case CMD:
			handler_cmd_mode(&curcmdframe);
			break;
		case EXIT:
			break;
		default:
			break;
	}
}

//터미널 초기화
void init_terminal()
{
	//리스트 초기화
	list_init(&filterIdList.head, &filterIdList.tail);
	list_init(&maskIdList.head, &maskIdList.tail);

	//시그널 핸들러 등록
	signal(SIGINT, handler_sigint);
}

void run_terminal()
{
	//초기화, 첫번째 상태는 CMD.
	tcgetattr(STDIN_FILENO, &old_settings); 
	set_cli_mode(CMD);

	while(1)
	{
		run_cli_mode();
	}
}
