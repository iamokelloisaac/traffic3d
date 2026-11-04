/*
 * config.c — config defaults and file parser.
 *
 * Commit: feat(config): implement defaults and key=value parser
 *
 * Depends on: config.h, <stdio.h>, <stdlib.h>, <string.h>, <ctype.h>, <errno.h>
 * Consumed by: main.
 *
 * The parser is deliberately simple:
 *   - one "key = value" per line
 *   - '#' starts a comment (whole-line or trailing)
 *   - surrounding whitespace is trimmed
 *   - strings, ints, doubles, hex colors all supported
 */
#include "traffic3d/config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

/* ─── Defaults ─────────────────────────────────────────────────── */
void config_defaults(config_t* cfg) {
    *cfg = (config_t){
        .pedestrian_call_delay_seconds = 2.0,
        .queued_call_delay_seconds     = 10.0, /* Default 10s wait */
        .queue_cool_off_seconds        = 30.0,
        .allow_mid_walk_join           = true,
        .min_crossing_time_seconds     = 5.0,
        .safety_margin_seconds         = 2.0,

        .vehicle_green_to_yellow_flash_hz     = 3.0,
        .vehicle_green_to_yellow_flash_cycles = 3,
        .vehicle_yellow_steady_seconds        = 3.0,
        .vehicle_yellow_to_red_flash_hz       = 3.0,
        .vehicle_yellow_to_red_flash_cycles   = 3,
        .vehicle_red_to_green_flash_hz        = 3.0,
        .vehicle_red_to_green_flash_cycles    = 3,

        .pedestrian_red_to_green_flash_hz     = 3.0,
        .pedestrian_red_to_green_flash_cycles = 3,
        .pedestrian_walk_seconds              = 15.0,
        .pedestrian_blink_seconds             = 5.0,
        .pedestrian_blink_hz                  = 2.0,
        .pedestrian_green_to_red_flash_hz     = 3.0,
        .pedestrian_green_to_red_flash_cycles = 3,

        .all_red_hold_seconds_1 = 1.0,
        .all_red_hold_seconds_2 = 1.0,

        .car_spawn_rate_per_second = 1.5,
        .car_speed_min             = 4.0,
        .car_speed_max             = 7.0,
        .car_speed_yellow_factor   = 0.4,
        .car_speed_stop_distance   = 2.0,
        .car_lane_count            = 2,

        .penguin_spawn_rate_per_second = 2.0,
        .penguin_speed_min             = 0.4,
        .penguin_speed_max             = 0.8,
        .penguin_max_on_screen         = 40,
        .penguin_hesitate_min          = 0.2,
        .penguin_hesitate_max          = 1.5,
        .penguin_accessory_chance      = 0.05,
        .penguin_family_chance         = 0.40,

        .color_vehicle_go     = 0x00DC50,
        .color_vehicle_warn   = 0xFFBE28,
        .color_vehicle_stop   = 0xDC2828,
        .color_pedestrian_go  = 0x00DC50,
        .color_pedestrian_stop= 0xDC2828,
        .color_sky_top        = 0x0A0A1E,
        .color_sky_bottom     = 0x3C285A,
        .color_horizon_glow   = 0x50407A,
        .color_road           = 0x1E1E28,
        .color_road_marking   = 0xDCDCDC,
        .color_sidewalk       = 0x5A5A64,
        .color_building       = 0x323246,
        .color_window_lit     = 0xFFDC78,

        .camera_orbit_period_seconds = 60.0,
        .star_count                  = 80,
        .building_count              = 5,
        .tree_count                  = 6,

        .key_pedestrian_call = 'p',
        .key_quit            = 'e',
    };
}

/* ─── Parser helpers ───────────────────────────────────────────── */

static char* trim_inplace(char* s) {
    while (isspace((unsigned char)*s)) ++s;
    if (*s == '\0') return s;
    char* end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char)*end)) *end-- = '\0';
    return s;
}

/* Strip trailing '#' comment (naive — no string literals in our file). */
static void strip_comment(char* s) {
    char* hash = strchr(s, '#');
    if (hash) *hash = '\0';
}

static bool parse_double(const char* val, double* out) {
    errno = 0;
    char* endp = NULL;
    double d = strtod(val, &endp);
    if (endp == val || errno != 0) return false;
    *out = d;
    return true;
}

static bool parse_int(const char* val, int* out) {
    errno = 0;
    char* endp = NULL;
    long v = strtol(val, &endp, 0);
    if (endp == val || errno != 0) return false;
    *out = (int)v;
    return true;
}

/* Handle single characters, with or without quotes (e.g. 'p' or p) */
static bool parse_char(const char* val, int* out) {
    if (val[0] == '\'' || val[0] == '"') {
        if (val[2] == val[0]) { *out = val[1]; return true; }
    }
    if (val[1] == '\0') { *out = val[0]; return true; }
    return parse_int(val, out);
}

static bool parse_bool(const char* val, bool* out) {
    if (!strcasecmp(val, "true")  || !strcmp(val, "1")) { *out = true;  return true; }
    if (!strcasecmp(val, "false") || !strcmp(val, "0")) { *out = false; return true; }
    return false;
}

static bool parse_color(const char* val, uint32_t* out) {
    /* Accept "0xRRGGBB", "#RRGGBB", or decimal. */
    const char* start = val;
    if (*start == '#') ++start;
    errno = 0;
    char* endp = NULL;
    unsigned long v = strtoul(start, &endp, 0); /* base 0 handles 0x prefix */
    if (endp == start || errno != 0) return false;
    *out = (uint32_t)(v & 0xFFFFFF);
    return true;
}

/* ─── Key dispatcher ──────────────────────────────────────────── */

#define TRY_DOUBLE(key, field) \
    if (!strcmp(k, key)) { if (parse_double(v, &cfg->field)) return true; return false; }
#define TRY_INT(key, field) \
    if (!strcmp(k, key)) { if (parse_int(v, &cfg->field)) return true; return false; }
#define TRY_BOOL(key, field) \
    if (!strcmp(k, key)) { if (parse_bool(v, &cfg->field)) return true; return false; }
#define TRY_COLOR(key, field) \
    if (!strcmp(k, key)) { if (parse_color(v, &cfg->field)) return true; return false; }
#define TRY_CHAR(key, field) \
    if (!strcmp(k, key)) { if (parse_char(v, &cfg->field)) return true; return false; }


#define TRY_CHAR(key, field) \
    if (!strcmp(k, key)) { if (parse_char(v, &cfg->field)) return true; return false; }

static bool config_set(config_t* cfg, const char* k, const char* v) {
    /* Timings */
    TRY_DOUBLE("pedestrian_call_delay_seconds", pedestrian_call_delay_seconds)
    TRY_DOUBLE("queued_call_delay_seconds",     queued_call_delay_seconds)
    TRY_DOUBLE("queue_cool_off_seconds",        queue_cool_off_seconds)
    TRY_BOOL  ("allow_mid_walk_join",           allow_mid_walk_join)
    TRY_DOUBLE("min_crossing_time_seconds",     min_crossing_time_seconds)
    TRY_DOUBLE("safety_margin_seconds",         safety_margin_seconds)

    TRY_DOUBLE("vehicle_green_to_yellow_flash_hz",     vehicle_green_to_yellow_flash_hz)
    TRY_INT   ("vehicle_green_to_yellow_flash_cycles", vehicle_green_to_yellow_flash_cycles)
    TRY_DOUBLE("vehicle_yellow_steady_seconds",        vehicle_yellow_steady_seconds)
    TRY_DOUBLE("vehicle_yellow_to_red_flash_hz",       vehicle_yellow_to_red_flash_hz)
    TRY_INT   ("vehicle_yellow_to_red_flash_cycles",   vehicle_yellow_to_red_flash_cycles)
    TRY_DOUBLE("vehicle_red_to_green_flash_hz",        vehicle_red_to_green_flash_hz)
    TRY_INT   ("vehicle_red_to_green_flash_cycles",    vehicle_red_to_green_flash_cycles)

    TRY_DOUBLE("pedestrian_red_to_green_flash_hz",     pedestrian_red_to_green_flash_hz)
    TRY_INT   ("pedestrian_red_to_green_flash_cycles", pedestrian_red_to_green_flash_cycles)
    TRY_DOUBLE("pedestrian_walk_seconds",              pedestrian_walk_seconds)
    TRY_DOUBLE("pedestrian_blink_seconds",             pedestrian_blink_seconds)
    TRY_DOUBLE("pedestrian_blink_hz",                  pedestrian_blink_hz)
    TRY_DOUBLE("pedestrian_green_to_red_flash_hz",     pedestrian_green_to_red_flash_hz)
    TRY_INT   ("pedestrian_green_to_red_flash_cycles", pedestrian_green_to_red_flash_cycles)

    TRY_DOUBLE("all_red_hold_seconds_1", all_red_hold_seconds_1)
    TRY_DOUBLE("all_red_hold_seconds_2", all_red_hold_seconds_2)

    TRY_DOUBLE("car_spawn_rate_per_second", car_spawn_rate_per_second)
    TRY_DOUBLE("car_speed_min",             car_speed_min)
    TRY_DOUBLE("car_speed_max",             car_speed_max)
    TRY_DOUBLE("car_speed_yellow_factor",   car_speed_yellow_factor)
    TRY_DOUBLE("car_speed_stop_distance",   car_speed_stop_distance)
    TRY_INT   ("car_lane_count",            car_lane_count)

    TRY_DOUBLE("penguin_spawn_rate_per_second", penguin_spawn_rate_per_second)
    TRY_DOUBLE("penguin_speed_min",             penguin_speed_min)
    TRY_DOUBLE("penguin_speed_max",             penguin_speed_max)
    TRY_INT   ("penguin_max_on_screen",         penguin_max_on_screen)
    TRY_DOUBLE("penguin_hesitate_min",          penguin_hesitate_min)
    TRY_DOUBLE("penguin_hesitate_max",          penguin_hesitate_max)
    TRY_DOUBLE("penguin_accessory_chance",      penguin_accessory_chance)
    TRY_DOUBLE("penguin_family_chance",         penguin_family_chance)

    TRY_COLOR ("color_vehicle_go",     color_vehicle_go)
    TRY_COLOR ("color_vehicle_warn",   color_vehicle_warn)
    TRY_COLOR ("color_vehicle_stop",   color_vehicle_stop)
    TRY_COLOR ("color_pedestrian_go",  color_pedestrian_go)
    TRY_COLOR ("color_pedestrian_stop",color_pedestrian_stop)
    TRY_COLOR ("color_sky_top",        color_sky_top)
    TRY_COLOR ("color_sky_bottom",     color_sky_bottom)
    TRY_COLOR ("color_horizon_glow",   color_horizon_glow)
    TRY_COLOR ("color_road",           color_road)
    TRY_COLOR ("color_road_marking",   color_road_marking)
    TRY_COLOR ("color_sidewalk",       color_sidewalk)
    TRY_COLOR ("color_building",       color_building)
    TRY_COLOR ("color_window_lit",     color_window_lit)

    TRY_DOUBLE("camera_orbit_period_seconds", camera_orbit_period_seconds)
    TRY_INT   ("star_count",                  star_count)
    TRY_INT   ("building_count",              building_count)
    TRY_INT   ("tree_count",                  tree_count)

    TRY_CHAR  ("key_pedestrian_call", key_pedestrian_call)
    TRY_CHAR  ("key_quit",            key_quit)

    return false; /* unknown key */
}

/* ─── Public loader ──────────────────────────────────────────── */

bool config_load(config_t* cfg, const char* path) {
    config_defaults(cfg);
    if (!path) return false;

    FILE* f = fopen(path, "r");
    if (!f) return false;

    char line[512];
    int lineno = 0;
    while (fgets(line, sizeof line, f)) {
        ++lineno;
        strip_comment(line);
        char* s = trim_inplace(line);
        if (*s == '\0') continue;

        char* eq = strchr(s, '=');
        if (!eq) {
            fprintf(stderr,
                    "[config] %s:%d: missing '=', skipping\n", path, lineno);
            continue;
        }
        *eq = '\0';
        char* key = trim_inplace(s);
        char* val = trim_inplace(eq + 1);

        if (!config_set(cfg, key, val)) {
            fprintf(stderr,
                    "[config] %s:%d: unknown key '%s'\n", path, lineno, key);
        }
    }
    fclose(f);
    return true;
}