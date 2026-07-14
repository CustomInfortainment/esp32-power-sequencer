#ifndef PROFILING_H
#define PROFILING_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "global.h"

static inline uint64_t now_ns(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + (uint64_t) ts.tv_nsec;
}
double metric_report_fps(ProfilingFrame* frame);
void metric_report_ns(uint64_t* elapsed_sum, uint64_t* elapsed_cnt);

#endif