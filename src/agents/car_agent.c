/*
 * car_agent.c — Implementation of vehicle movement, queueing, and reversing.
 *
 * Commit: feat(agents): implement reverse gear to clear the zebra crossing
 *
 * Depends on: car_agent.h
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

    uint32_t colors[] = {0xCC3333, 0x3366CC, 0xCCCC33, 0x33CC66, 0xCCCCCC, 0x333333, 0xAA44AA};
    car->color = colors[seed % 7];
}

void car_agent_update(car_agent_t* car, car_agent_t* all_cars, int num_cars, double dt, light_state_t light, const config_t* cfg) {
    float speed_limit = cfg->car_speed_max;

    /* Define the Zebra Crossing zone (Z=10 to Z=14) plus car half-length */
    bool on_zebra = false;
    if (car->lane == 1 && car->position.z > 8.0f && car->position.z < 16.0f) on_zebra = true;
    if (car->lane == -1 && car->position.z > 8.0f && car->position.z < 16.0f) on_zebra = true;

    /* 1. Base Speed Logic */
    if (light == LIGHT_RED || light == LIGHT_YELLOW) {
        if (on_zebra) {
            /* REVERSE GEAR: Back up to clear the zebra */
            speed_limit = -cfg->car_speed_min;
        } else {
            /* Normal stop */
            speed_limit = 0.0f;
        }
    }

    /* 2. Queue Logic (Don't hit the car in front) */
    float min_gap = 7.0f;
    for (int i = 0; i < num_cars; i++) {
        if (i == (int)(car - all_cars)) continue;
        car_agent_t* other = &all_cars[i];

        if (other->lane != car->lane) continue;

        float dist = (car->lane == 1) ? (other->position.z - car->position.z) : (car->position.z - other->position.z);

        if (dist > 0.0f && dist < min_gap) {
            if (other->current_speed < 0.0f) {
                /* The car in front is reversing! We must reverse too to give it room. */
                speed_limit = -cfg->car_speed_min;
            } else if (other->current_speed == 0.0f && dist < 4.0f) {
                /* The car in front is stopped and we are too close. */
                speed_limit = 0.0f;
            } else if (other->current_speed > 0.0f) {
                /* Normal queueing */
                if (other->current_speed < speed_limit) speed_limit = other->current_speed;
                if (dist < 3.5f) speed_limit = 0.0f;
            }
        }
    }

    /* 3. Apply speed changes */
    float accel = 10.0f;
    if (car->current_speed < speed_limit) {
        car->current_speed += accel * (float)dt;
        if (car->current_speed > speed_limit) car->current_speed = speed_limit;
    } else if (car->current_speed > speed_limit) {
        car->current_speed -= accel * 2.0f * (float)dt;
        if (car->current_speed < speed_limit) car->current_speed = speed_limit;
    }

    /* 4. Move */
    float dir = (car->lane == 1) ? 1.0f : -1.0f;
    car->position.z += car->current_speed * dir * (float)dt;

    /* 5. Respawn Loop */
    if (car->lane == 1 && car->position.z > 80.0f) car->position.z = -80.0f;
    else if (car->lane == -1 && car->position.z < -80.0f) car->position.z = 80.0f;
}