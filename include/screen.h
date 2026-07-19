#ifndef SCREEN_H
#define SCREEN_H

#include <locale.h>
#include <ncurses.h>

#include "global.h"
#include "file.h"

typedef struct Vector2 
{
    int x;
    int y;
} Vector2;

typedef struct WINDOW_INFO 
{
    Vector2 size;
    Vector2 pos;
    WINDOW *window;
} WINDOW_INFO;

void get_terminal_env(Vector2 *v2);
void set_window(WINDOW_INFO *window, int height, int width, int y, int x);

void display_tui();
void display_title();
void display_can_frame(CANFrame* frame);
void display_metric(char *frame, char *elapsed_time);

void init_screen();
void run_screen();

#endif
