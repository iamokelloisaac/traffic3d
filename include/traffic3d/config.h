/*
 * config.h — runtime configuration loaded from a .conf file.
 *
 * Commit: feat(config): add config struct with timings, colors, and defaults
 *
 * Depends on: common.h
 * Consumed by: main, traffic, agents, scene, render (for colors and timings).
 *
 * Design:
 *   - config_defaults() fills a struct with sensible defaults.
 *   - config_load(path) parses the file (if it exists) OVER the defaults.
 *   - Missing file is not an error — defaults are always valid.
 *
 * All timing values are in seconds. All colors are 0xRRGGBB.
 */
#ifndef TRAFFIC3D_CONFIG_H
#define TRAFFIC3D_CONFIG_H

#include "traffic3d/common.h"
#include <stddef.h>

typedef struct {
    /* ── PEDESTRIAN CALL ───────────────────────────────── */
    double pedestrian_call_delay_seconds;     /* button → yellow begins */
    double queued_call_delay_seconds;         /* wait time after green before queued call fires */
    double queue_cool_off_seconds;            /* queued call wait */
    bool   allow_mid_walk_join;
    double min_crossing_time_seconds;         /* slowest crossing */
    double safety_margin_seconds;

    /* ── VEHICLE TRANSITIONS ───────────────────────────── */
    double vehicle_green_to_yellow_flash_hz;
    int    vehicle_green_to_yellow_flash_cycles;
    double vehicle_yellow_steady_seconds;
    double vehicle_yellow_to_red_flash_hz;
    int    vehicle_yellow_to_red_flash_cycles;
    double vehicle_red_to_green_flash_hz;
    int    vehicle_red_to_green_flash_cycles;

    /* ── PEDESTRIAN TRANSITIONS ────────────────────────── */
    double pedestrian_red_to_green_flash_hz;
    int    pedestrian_red_to_green_flash_cycles;
    double pedestrian_walk_seconds;
    double pedestrian_blink_seconds;
    double pedestrian_blink_hz;
    double pedestrian_green_to_red_flash_hz;
    int    pedestrian_green_to_red_flash_cycles;

    /* ── ALL-RED SAFETY GAPS ───────────────────────────── */
    double all_red_hold_seconds_1;
    double all_red_hold_seconds_2;

    /* ── CARS ──────────────────────────────────────────── */
    double car_spawn_rate_per_second;
    double car_speed_min;
    double car_speed_max;
    double car_speed_yellow_factor;           /* speed × this on yellow */
    double car_speed_stop_distance;
    int    car_lane_count;

    /* ── PENGUINS ──────────────────────────────────────── */
    double penguin_spawn_rate_per_second;
    double penguin_speed_min;
    double penguin_speed_max;
    int    penguin_max_on_screen;
    double penguin_hesitate_min;
    double penguin_hesitate_max;
    double penguin_accessory_chance;          /* 0..1 */
    double penguin_family_chance;

    /* ── COLORS (0xRRGGBB) ─────────────────────────────── */
    uint32_t color_vehicle_go;
    uint32_t color_vehicle_warn;
    uint32_t color_vehicle_stop;
    uint32_t color_pedestrian_go;
    uint32_t color_pedestrian_stop;
    uint32_t color_sky_top;
    uint32_t color_sky_bottom;
    uint32_t color_horizon_glow;
    uint32_t color_road;
    uint32_t color_road_marking;
    uint32_t color_sidewalk;
    uint32_t color_building;
    uint32_t color_window_lit;

    /* ── SCENE ─────────────────────────────────────────── */
    double camera_orbit_period_seconds;
    int    star_count;
    int    building_count;
    int    tree_count;

    /* ── INPUT ─────────────────────────────────────────── */
    int    key_pedestrian_call;               /* e.g. 'p' */
    int    key_quit;                          /* e.g. 'e' */
} config_t;

/* Fill *cfg with sensible defaults. Safe to call on uninitialized memory. */
void config_defaults(config_t* cfg);

/* Attempt to load from path. Lines of "key = value", '#' for comments.
 * Unknown keys are warned to stderr but not fatal.
 * Returns true if the file was opened; false if it did not exist.
 * In both cases, *cfg is fully populated (defaults + any parsed overrides). */
bool config_load(config_t* cfg, const char* path);

#endif /* TRAFFIC3D_CONFIG_H */