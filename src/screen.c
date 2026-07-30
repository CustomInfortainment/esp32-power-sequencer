#include "screen.h"

// 현재 터미널 창 크기
static Vector2 terminal_size;

// 최상위 윈도우
static WINDOW_INFO LOG_WINDOW;
static WINDOW_INFO METRIC_WINDOW;
static WINDOW_INFO INPUT_WINDOW;

// 타이틀
static WINDOW_INFO CAN_LOG_TITLE;
static WINDOW_INFO METRIC_TITLE;

//정보 출력
static WINDOW_INFO CAN_LOG_INFO;
static WINDOW_INFO METRIC_LOG_INFO;

static int final_yaxis_idx = 0;

void get_terminal_env(Vector2 *v2) 
{
    int width = 0;
    int height = 0;

    getmaxyx(stdscr, height, width);

    v2->y = height;
    v2->x = width;
}

void set_window(WINDOW_INFO *window, int height, int width, int y, int x) 
{
    window->pos.y = y;
    window->pos.x = x;
    window->size.y = height;
    window->size.x = width;
}

void display_tui() 
{
    set_window(&LOG_WINDOW, terminal_size.y, terminal_size.x * 0.7f, 0, 0);
    set_window(&METRIC_WINDOW, terminal_size.y, terminal_size.x * 0.3f, 0, LOG_WINDOW.size.x);

    LOG_WINDOW.window = newwin(LOG_WINDOW.size.y, LOG_WINDOW.size.x, 0, 0);
    METRIC_WINDOW.window = newwin(METRIC_WINDOW.size.y, METRIC_WINDOW.size.x, 0, LOG_WINDOW.size.x);

    box(LOG_WINDOW.window, '|', '-');
    box(METRIC_WINDOW.window, '|', '-');

    wrefresh(LOG_WINDOW.window);
    wrefresh(METRIC_WINDOW.window);
}

void display_title()
{
    set_window(&CAN_LOG_TITLE, LOG_WINDOW.size.y * 0.1f, LOG_WINDOW.size.x, 0, 0);
    CAN_LOG_TITLE.window = derwin(LOG_WINDOW.window, CAN_LOG_TITLE.size.y, CAN_LOG_TITLE.size.x, 0, 0);

    set_window(&METRIC_TITLE, METRIC_WINDOW.size.y * 0.1f, METRIC_WINDOW.size.x, 0, 0);
    METRIC_TITLE.window = derwin(METRIC_WINDOW.window, METRIC_TITLE.size.y, METRIC_TITLE.size.x, 0, 0);

    box(CAN_LOG_TITLE.window, '|', '*');
    box(METRIC_TITLE.window, '|', '*');

    mvwprintw(CAN_LOG_TITLE.window, 1, 1, "CAN_FRAME_LOG");
    mvwprintw(METRIC_TITLE.window, 1, 1, "METRIC_INFO_LOG");

    wrefresh(CAN_LOG_TITLE.window);
    wrefresh(METRIC_TITLE.window);
}

void display_metric(PRT_LOG_STRUCTURE* log_structure)
{
    char title[256];
    int yaxis[256]; 

    set_window(&METRIC_LOG_INFO, METRIC_WINDOW.size.y * 0.9f, METRIC_WINDOW.size.x, 0, 0);
    METRIC_LOG_INFO.window = derwin(METRIC_WINDOW.window, METRIC_LOG_INFO.size.y, METRIC_LOG_INFO.size.x, 0, 0);

    if(log_structure == NULL) return;

    switch(log_structure->structure_type)
    {
        case CAN_FRAME_FPS:
            strcpy(title, "CAN_FRAME_FPS");
            break;
        case SAVE_ELAPSED_TIME:
            strcpy(title, "SAVE_ELAPSED_TIME");
            break;
    }

    for(int i = 1; i < 256; i++)
    {
        yaxis[i] = i + 2;
    }

    if(log_structure->yaxis_idx == 0)
    {
        final_yaxis_idx++;
        log_structure->yaxis_idx = final_yaxis_idx;
    }
    
    wclrtoeol(METRIC_LOG_INFO.window);

    mvwprintw(METRIC_LOG_INFO.window, yaxis[log_structure->yaxis_idx], 1, "%d. %s : %lf", log_structure->yaxis_idx, title, log_structure->log_value);
    wrefresh(METRIC_LOG_INFO.window);
}

void init_screen() 
{
    setlocale(LC_ALL, "");
    initscr();

    nodelay(stdscr, TRUE);

    get_terminal_env(&terminal_size);
    refresh();
}

void run_screen()
{
    display_tui();
    display_title();
}
