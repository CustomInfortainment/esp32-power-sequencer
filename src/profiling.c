#include "profiling.h"
#include "screen.h"

static uint64_t last_report_ns;

static PRT_LOG_STRUCTURE can_frame_fps;
static PRT_LOG_STRUCTURE save_elapsed_time;

void metric_report_fps(ProfilingFrame* frame)
{
    uint64_t now = now_ns();
    double fps = 0;

    if(frame->last_report == 0)
    {
        frame->last_report = now;
        return;
    }

    uint64_t elapsed_ns = now - frame->last_report;

    if(elapsed_ns >= 1000000000ULL)
    {
        frame->now_sec = elapsed_ns / 1e9;
        fps = frame->g_frame_cnt / frame->now_sec;
        frame->g_frame_cnt = 0;
        frame->last_report = now;
    }
    can_frame_fps.structure_type = CAN_FRAME_FPS;
    can_frame_fps.log_value = fps;

    if(fps != 0)
        display_metric(&can_frame_fps);
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
        save_elapsed_time.structure_type = SAVE_ELAPSED_TIME;
        save_elapsed_time.log_value = (double)*elapsed_sum / *elapsed_cnt;

        display_metric(&save_elapsed_time); 

        *elapsed_sum = 0;
        *elapsed_cnt = 0;
    }
}
