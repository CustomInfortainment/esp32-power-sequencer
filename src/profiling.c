#include "profiling.h"

static uint64_t last_report_ns;

double metric_report_fps(ProfilingFrame* frame)
{
    uint64_t now = now_ns();
    double fps = 0;

    if(frame->last_report == 0)
    {
        frame->last_report = now;
        return 0;
    }

    uint64_t elapsed_ns = now - frame->last_report;

    if(elapsed_ns >= 1000000000ULL)
    {
        frame->now_sec = elapsed_ns / 1e9;
        fps = frame->g_frame_cnt / frame->now_sec;
        frame->g_frame_cnt = 0;
        frame->last_report = now;
    }

    return fps;
}

void metric_report_ns(uint64_t* elapsed_sum, uint64_t* elapsed_cnt)
{
    uint64_t now = now_ns();
    if(last_report_ns == 0)
    {
        last_report_ns = now;
        return;
    }

    uint64_t elapsed_ns = now - last_report_ns;

    if(elapsed_ns >= 1000000000ULL)
    {
        last_report_ns = now;
        fprintf(stderr, "elapsed ns : %llu\n", *elapsed_sum / *elapsed_cnt);

        *elapsed_sum = 0;
        *elapsed_cnt = 0;
    }
}