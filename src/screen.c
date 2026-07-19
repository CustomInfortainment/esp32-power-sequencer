#include "screen.h"
#include "global.h"
#include <ncurses.h>

// 현재 터미널 창 크기
static Vector2 terminal_size;

// 최상위 윈도우
static WINDOW_INFO LOG_WINDOW;
static WINDOW_INFO METRIC_WINDOW;
static WINDOW_INFO INPUT_WINDOW;

// 타이틀
static WINDOW_INFO CAN_LOG_TITLE;
static WINDOW_INFO METRIC_TITLE;

// 정보 출력
static WINDOW_INFO CAN_LOG;
static WINDOW_INFO METRIC_LOG;

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

    wrefresh(CAN_LOG_TITLE.window);
    wrefresh(METRIC_TITLE.window);
}

void display_can_frame(CANFrame* frame)
{
    
}

void display_metric(char *frame, char *elapsed_time)
{

}

void init_screen() 
{
    setlocale(LC_ALL, "");
    initscr();

    get_terminal_env(&terminal_size);
    refresh();
}

void run_screen()
{
    display_tui();
    getch();
    endwin();
}
