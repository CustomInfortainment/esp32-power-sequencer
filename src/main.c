#include <stdio.h>
#include "file.h"
#include "sniffing.h"
#include "utility.h"
#include "terminal.h"
#include "screen.h"

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
}

void do_test()
{
    sniff_init();

    while(1)
    {
        sniff_data_recv();
    }
}

int main(void)
{
    //---------터미널 작동 호출부-----------
    //tool_sniffing();
    //tool_terminal();

    //do_test();

    tool_screen();


    return 0;
}
