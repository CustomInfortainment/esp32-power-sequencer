#ifndef SCREEN_H
#define SCREEN_H

#include <ncurses.h>
#include <locale.h>

typedef struct Vector2
{
    int x;
    int y;
} Vector2;

typedef struct WINDOW_INFO
{
    Vector2 size;
    Vector2 pos;
    WINDOW* window;
} WINDOW_INFO;

void get_terminal_env(Vector2* v2);
void set_window(WINDOW_INFO* window, int height, int width, int y, int x);

void init_screen();
void exit_screen();

#endif