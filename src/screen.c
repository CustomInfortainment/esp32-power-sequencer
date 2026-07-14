#include "screen.h"

//현재 터미널 창 크기
static Vector2 terminal_size;

//최상위 윈도우
static WINDOW_INFO LOG_WINDOW;
static WINDOW_INFO STAT_WINDOW;
static WINDOW_INFO INPUT_WINDOW;

//타이틀
static WINDOW_INFO LOG_TITLE;
static WINDOW_INFO STATE_TITLE;

//정보 출력
static WINDOW_INFO LOG_INFO;
static WINDOW_INFO STATE_INFO;

void get_terminal_env(Vector2* v2)
{
    int width = 0;
    int height = 0;

    getmaxyx(stdscr, height, width);

    v2->y = height;
    v2->x = width;
}

void set_window(WINDOW_INFO* window, int height, int width, int y, int x)
{
    window->pos.y = y;
    window->pos.x = x;
    window->size.y = height;
    window->size.x = width;
}

void init_screen()
{
    setlocale(LC_ALL, "");
    initscr();

    get_terminal_env(&terminal_size);

    set_window(&LOG_WINDOW, terminal_size.y, terminal_size.x * 0.7f, 0, 0);
    set_window(&STAT_WINDOW, terminal_size.y, terminal_size.x * 0.3f, 0, LOG_WINDOW.size.x);

    LOG_WINDOW.window = newwin(LOG_WINDOW.size.y, LOG_WINDOW.size.x, 0, 0);
    STAT_WINDOW.window = newwin(STAT_WINDOW.size.y, STAT_WINDOW.size.x, 0, LOG_WINDOW.size.x);

    box(LOG_WINDOW.window, '|', '-');
    box(STAT_WINDOW.window, '|', '-');

    refresh();
    wrefresh(LOG_WINDOW.window);
    wrefresh(STAT_WINDOW.window);

    set_window(&LOG_TITLE, LOG_WINDOW.size.y * 0.1f, LOG_WINDOW.size.x, 0, 0);
    LOG_TITLE.window = derwin(LOG_WINDOW.window, LOG_TITLE.size.y, LOG_TITLE.size.x, 0, 0);

    box(LOG_TITLE.window, '|', '*');

    mvwprintw(LOG_TITLE.window, 1, 2, "로그 출력창");
    wrefresh(LOG_TITLE.window);

    getch();
    endwin();
}

void exit_screen()
{

}

