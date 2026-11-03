#ifndef TRAFFIC3D_TRAFFIC_STATE_MACHINE_H
#define TRAFFIC3D_TRAFFIC_STATE_MACHINE_H

#include "traffic3d/common.h"
#include "traffic3d/config.h"

typedef enum {
    PHASE_STEADY_GREEN,
    PHASE_CALL_DELAY,
    PHASE_VEH_GREEN_TO_YELLOW,
    PHASE_VEH_YELLOW_STEADY,
    PHASE_VEH_YELLOW_TO_RED,
    PHASE_ALL_RED_1,
    PHASE_PED_RED_TO_GREEN,
    PHASE_PED_WALK,
    PHASE_PED_BLINK,
    PHASE_PED_GREEN_TO_RED,
    PHASE_ALL_RED_2,
    PHASE_VEH_RED_TO_GREEN
} traffic_phase_t;

typedef struct {
    traffic_phase_t current_phase;
    double elapsed_in_phase;
    bool pedestrian_call_pending;
    bool queued_call;
} traffic_sm_t;

void traffic_sm_init(traffic_sm_t* sm);
void traffic_sm_update(traffic_sm_t* sm, double dt, const config_t* cfg);
void traffic_sm_press_button(traffic_sm_t* sm, const config_t* cfg);
light_state_t traffic_sm_get_vehicle_light(const traffic_sm_t* sm, const config_t* cfg);
light_state_t traffic_sm_get_pedestrian_light(const traffic_sm_t* sm, const config_t* cfg);

#endif
