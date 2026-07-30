#include "sniffing.h"
#include "terminal.h"
#include "screen.h"
#include <ncurses.h>

void tool_terminal()
{
    init_terminal();
    run_terminal();
}

void tool_sniffing()
{
    sniff_init();
}

void tool_screen()
{
    init_screen();
    run_screen();
}

void do_test()
{
    sniff_init();
    tool_screen();

    while(1)
    {
        sniff_data_recv();
        int key = getch();

        if(key == 'q') break;
    }
    endwin();
}

int main(void)
{
    //---------터미널 작동 호출부-----------
    //tool_sniffing();
    //tool_terminal();

    do_test();

    return 0;
}
