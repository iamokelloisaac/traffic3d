/*
* timing.h — monotonic clock + sleep wrappers.
 *
 * Commit: feat(platform): add monotonic timing primitives
 *
 * Depends on: <time.h>
 * Consumed by: main, agents, traffic.
 */
#ifndef TRAFFIC3D_PLATFORM_TIMING_H
#define TRAFFIC3D_PLATFORM_TIMING_H

/* Seconds since an arbitrary monotonic origin. Never goes backwards. */
double timing_now_seconds();

/* Sleep for at least `seconds`. Safe with values <0 (no-op). */
void timing_sleep_seconds(double seconds);

#endif