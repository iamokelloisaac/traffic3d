#include "traffic3d/traffic/state_machine.h"
#include <math.h>

static bool is_flash_on(double elapsed, double hz) {
    return fmod(elapsed * hz, 1.0) < 0.5;
}

void traffic_sm_init(traffic_sm_t* sm) {
    sm->current_phase = PHASE_STEADY_GREEN;
    sm->elapsed_in_phase = 0.0;
    sm->pedestrian_call_pending = false;
    sm->queued_call = false;
    sm->last_press_time = -100.0; /* Set far in the past */
}

void traffic_sm_press_button(traffic_sm_t* sm, const config_t* cfg, double current_time) {
    (void)cfg;

    /* 1. Debounce: Ignore presses within 0.3 seconds of the last press */
    if (current_time - sm->last_press_time < 0.3) return;
    sm->last_press_time = current_time;

    /* 2. Explicit Queue Logic */
    if (sm->current_phase == PHASE_STEADY_GREEN) {
        /* First press: Start the sequence immediately. DO NOT queue. */
        sm->pedestrian_call_pending = true;
    } else {
        /* Multi-press during a cycle: Explicitly queue for later. */
        sm->queued_call = true;
    }
}

void traffic_sm_update(traffic_sm_t* sm, double dt, const config_t* cfg) {
    sm->elapsed_in_phase += dt;

    switch (sm->current_phase) {
        case PHASE_STEADY_GREEN:
            if (sm->pedestrian_call_pending) {
                sm->current_phase = PHASE_CALL_DELAY;
                sm->elapsed_in_phase = 0.0;
                sm->pedestrian_call_pending = false;
            } else if (sm->queued_call && sm->elapsed_in_phase >= cfg->queued_call_delay_seconds) {
                /* The wait time is over, trigger the batched queue */
                sm->current_phase = PHASE_CALL_DELAY;
                sm->elapsed_in_phase = 0.0;
                sm->queued_call = false;
            }
            break;
        case PHASE_CALL_DELAY:
            if (sm->elapsed_in_phase >= cfg->pedestrian_call_delay_seconds) {
                sm->current_phase = PHASE_VEH_GREEN_TO_YELLOW;
                sm->elapsed_in_phase = 0.0;
            }
            break;
        case PHASE_VEH_GREEN_TO_YELLOW: {
            double flash_time = (double)cfg->vehicle_green_to_yellow_flash_cycles / cfg->vehicle_green_to_yellow_flash_hz;
            if (sm->elapsed_in_phase >= flash_time) {
                sm->current_phase = PHASE_VEH_YELLOW_STEADY;
                sm->elapsed_in_phase = 0.0;
            }
            break;
        }
        case PHASE_VEH_YELLOW_STEADY:
            if (sm->elapsed_in_phase >= cfg->vehicle_yellow_steady_seconds) {
                sm->current_phase = PHASE_VEH_YELLOW_TO_RED;
                sm->elapsed_in_phase = 0.0;
            }
            break;
        case PHASE_VEH_YELLOW_TO_RED: {
            double flash_time = (double)cfg->vehicle_yellow_to_red_flash_cycles / cfg->vehicle_yellow_to_red_flash_hz;
            if (sm->elapsed_in_phase >= flash_time) {
                sm->current_phase = PHASE_ALL_RED_1;
                sm->elapsed_in_phase = 0.0;
            }
            break;
        }
        case PHASE_ALL_RED_1:
            if (sm->elapsed_in_phase >= cfg->all_red_hold_seconds_1) {
                sm->current_phase = PHASE_PED_RED_TO_GREEN;
                sm->elapsed_in_phase = 0.0;
            }
            break;
        case PHASE_PED_RED_TO_GREEN: {
            double flash_time = (double)cfg->pedestrian_red_to_green_flash_cycles / cfg->pedestrian_red_to_green_flash_hz;
            if (sm->elapsed_in_phase >= flash_time) {
                sm->current_phase = PHASE_PED_WALK;
                sm->elapsed_in_phase = 0.0;
            }
            break;
        }
        case PHASE_PED_WALK:
            if (sm->elapsed_in_phase >= cfg->pedestrian_walk_seconds) {
                sm->current_phase = PHASE_PED_BLINK;
                sm->elapsed_in_phase = 0.0;
            }
            break;
        case PHASE_PED_BLINK:
            if (sm->elapsed_in_phase >= cfg->pedestrian_blink_seconds) {
                sm->current_phase = PHASE_PED_GREEN_TO_RED;
                sm->elapsed_in_phase = 0.0;
            }
            break;
        case PHASE_PED_GREEN_TO_RED: {
            double flash_time = (double)cfg->pedestrian_green_to_red_flash_cycles / cfg->pedestrian_green_to_red_flash_hz;
            if (sm->elapsed_in_phase >= flash_time) {
                sm->current_phase = PHASE_ALL_RED_2;
                sm->elapsed_in_phase = 0.0;
            }
            break;
        }
        case PHASE_ALL_RED_2:
            if (sm->elapsed_in_phase >= cfg->all_red_hold_seconds_2) {
                sm->current_phase = PHASE_VEH_RED_TO_GREEN;
                sm->elapsed_in_phase = 0.0;
            }
            break;
        case PHASE_VEH_RED_TO_GREEN: {
            double flash_time = (double)cfg->vehicle_red_to_green_flash_cycles / cfg->vehicle_red_to_green_flash_hz;
            if (sm->elapsed_in_phase >= flash_time) {
                sm->current_phase = PHASE_STEADY_GREEN;
                sm->elapsed_in_phase = 0.0;
            }
            break;
        }
    }
}

light_state_t traffic_sm_get_vehicle_light(const traffic_sm_t* sm, const config_t* cfg) {
    switch (sm->current_phase) {
        case PHASE_STEADY_GREEN:
        case PHASE_CALL_DELAY:
            return LIGHT_GREEN;
        case PHASE_VEH_GREEN_TO_YELLOW:
            return is_flash_on(sm->elapsed_in_phase, cfg->vehicle_green_to_yellow_flash_hz) ? LIGHT_YELLOW : LIGHT_GREEN;
        case PHASE_VEH_YELLOW_STEADY:
            return LIGHT_YELLOW;
        case PHASE_VEH_YELLOW_TO_RED:
            return is_flash_on(sm->elapsed_in_phase, cfg->vehicle_yellow_to_red_flash_hz) ? LIGHT_RED : LIGHT_YELLOW;
        case PHASE_VEH_RED_TO_GREEN:
            return is_flash_on(sm->elapsed_in_phase, cfg->vehicle_red_to_green_flash_hz) ? LIGHT_GREEN : LIGHT_RED;
        default:
            return LIGHT_RED;
    }
}

light_state_t traffic_sm_get_pedestrian_light(const traffic_sm_t* sm, const config_t* cfg) {
    switch (sm->current_phase) {
        case PHASE_PED_WALK:
            return LIGHT_GREEN;
        case PHASE_PED_BLINK:
            return is_flash_on(sm->elapsed_in_phase, cfg->pedestrian_blink_hz) ? LIGHT_GREEN : LIGHT_RED;
        case PHASE_PED_RED_TO_GREEN:
            return is_flash_on(sm->elapsed_in_phase, cfg->pedestrian_red_to_green_flash_hz) ? LIGHT_GREEN : LIGHT_RED;
        case PHASE_PED_GREEN_TO_RED:
            return is_flash_on(sm->elapsed_in_phase, cfg->pedestrian_green_to_red_flash_hz) ? LIGHT_RED : LIGHT_GREEN;
        default:
            return LIGHT_RED;
    }
}
