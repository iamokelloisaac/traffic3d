/*
* timing.c — monotonic clock + sleep implementation.
 *
 * Commit: feat(platform): implement timing with clock_gettime(CLOCK_MONOTONIC)
 *
 * Depends on: timing.h
 */
#include "traffic3d/platform/timing.h"
#include <time.h>

double timing_now_seconds(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
}

void timing_sleep_seconds(double seconds) {
    if (seconds <= 0.0) return;
    struct timespec ts;
    ts.tv_sec  = (time_t)seconds;
    ts.tv_nsec = (long)((seconds - (double)ts.tv_sec) * 1e9);
    /* Loop to handle EINTR safely. */
    while (nanosleep(&ts, &ts) == -1) { /* interrupted; ts now holds remainder */ }
}