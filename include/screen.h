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

enum STRUCTURE_TYPE
{
    CAN_FRAME_FPS,
    SAVE_ELAPSED_TIME
};

typedef struct PRT_LOG_STRUCTURE
{
    enum STRUCTURE_TYPE structure_type;
    double log_value;
    int yaxis_idx;
} PRT_LOG_STRUCTURE;

void get_terminal_env(Vector2 *v2);
void set_window(WINDOW_INFO *window, int height, int width, int y, int x);

void display_tui();
void display_title();
void display_can_frame(int id, int dlc, char* formatdata);
void display_metric(PRT_LOG_STRUCTURE* log_structure);

void init_screen();
void run_screen();

#endif
