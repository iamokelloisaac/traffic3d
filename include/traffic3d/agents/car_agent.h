/*
* car_agent.h — Vehicles moving along the road.
 *
 * Commit: fix(agents): add spawn offset, spacing, and queue logic for car stream
 */
#ifndef TRAFFIC3D_AGENTS_CAR_AGENT_H
#define TRAFFIC3D_AGENTS_CAR_AGENT_H

#include "traffic3d/common.h"
#include "traffic3d/config.h"
#include "traffic3d/math/vec3.h"

typedef struct {
    vec3_t position;
    float current_speed;
    float target_speed;
    int lane;           /* -1 for left lane, 1 for right lane */
    uint32_t color;
    size_t object_index; /* Link back to the scene graph */
} car_agent_t;

/* Initialize a car in a specific lane. start_z allows us to offset them. */
void car_agent_init(car_agent_t* car, int lane, float start_z, size_t obj_idx, const config_t* cfg, uint32_t seed);

/* Update position based on traffic light state and other cars in the lane */
void car_agent_update(car_agent_t* car, car_agent_t* all_cars, int num_cars, double dt, light_state_t light, const config_t* cfg);

#endif