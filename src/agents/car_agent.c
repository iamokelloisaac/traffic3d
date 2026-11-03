/*
 * car_agent.c — Implementation of vehicle movement, queueing, and respawning.
 *
 * Commit: fix(agents): implement proper stream spacing and queue braking
 */
#include "traffic3d/agents/car_agent.h"

void car_agent_init(car_agent_t* car, int lane, float start_z, size_t obj_idx, const config_t* cfg, uint32_t seed) {
    car->position = vec3(lane * 4.0f, 0.5f, start_z);
    car->lane = lane;
    car->object_index = obj_idx;

    float speed_range = cfg->car_speed_max - cfg->car_speed_min;
    float r = (float)(seed % 100) / 100.0f;
    car->current_speed = cfg->car_speed_min + (r * speed_range);
    car->target_speed = car->current_speed;

    /* Darker, more realistic car colors to contrast with the environment */
    uint32_t colors[] = {0xCC3333, 0x3366CC, 0xCCCC33, 0x33CC66, 0xCCCCCC, 0x333333, 0xAA44AA};
    car->color = colors[seed % 7];
}

void car_agent_update(car_agent_t* car, car_agent_t* all_cars, int num_cars, double dt, light_state_t light, const config_t* cfg) {
    float speed_limit = cfg->car_speed_max;

    /* 1. Light Logic */
    if (light == LIGHT_RED) {
        speed_limit = 0.0f;
    } else if (light == LIGHT_YELLOW) {
        speed_limit = cfg->car_speed_min * cfg->car_speed_yellow_factor;
    }

    /* 2. Queue Logic (Don't hit the car in front) */
    float min_gap = 7.0f;
    for (int i = 0; i < num_cars; i++) {
        if (i == (int)(car - all_cars)) continue; /* Don't check against self */
        car_agent_t* other = &all_cars[i];

        if (other->lane != car->lane) continue;

        float dist = (car->lane == 1) ? (other->position.z - car->position.z) : (car->position.z - other->position.z);

        /* If we are approaching a car ahead within our minimum gap */
        if (dist > 0.0f && dist < min_gap) {
            /* Match their speed to maintain a safe gap */
            if (other->current_speed < speed_limit) {
                speed_limit = other->current_speed;
            }
            if (dist < 3.0f && car->current_speed > 0.0f) {
                speed_limit = 0.0f; /* Hard stop to prevent collision */
            }
        }
    }

    /* 3. Stop Line Logic */
    float stop_z = (car->lane == 1) ? 6.0f : 16.0f;
    if (light == LIGHT_RED || light == LIGHT_YELLOW) {
        float dist_to_line = (car->lane == 1) ? (stop_z - car->position.z) : (car->position.z - stop_z);
        if (dist_to_line > 0.0f && dist_to_line < 8.0f) {
            speed_limit = 0.0f; /* Brake for the line */
        }
    }

    /* 4. Apply speed changes */
    float accel = 10.0f;
    if (car->current_speed < speed_limit) {
        car->current_speed += accel * (float)dt;
        if (car->current_speed > speed_limit) car->current_speed = speed_limit;
    } else if (car->current_speed > speed_limit) {
        car->current_speed -= accel * 2.0f * (float)dt; /* Brake faster than accelerate */
        if (car->current_speed < speed_limit) car->current_speed = speed_limit;
    }

    /* 5. Move */
    float dir = (car->lane == 1) ? 1.0f : -1.0f;
    car->position.z += car->current_speed * dir * (float)dt;

    /* 6. Respawn Loop */
    if (car->lane == 1 && car->position.z > 80.0f) {
        car->position.z = -80.0f;
    } else if (car->lane == -1 && car->position.z < -80.0f) {
        car->position.z = 80.0f;
    }
}