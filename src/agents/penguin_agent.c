/*
 * penguin_agent.c — Implementation of penguin crossing behavior.
 *
 * Commit: feat(agents): implement penguin waiting, crossing, and waddle animation
 *
 * Depends on: penguin_agent.h, <math.h>
 */
#include "traffic3d/agents/penguin_agent.h"
#include <math.h>

void penguin_agent_init(penguin_agent_t* p, int direction, size_t obj_idx, const config_t* cfg, uint32_t seed) {
    p->personality = personality_generate(seed, cfg);
    p->direction = direction;
    p->object_index = obj_idx;
    p->state = PENGUIN_WAITING_AT_CURB;
    p->waddle_phase = 0.0f;

    /* Spawn them on the sidewalk */
    float start_x = (direction == 1) ? -10.0f : 10.0f;
    float start_z = 12.0f + ((float)(seed % 50) / 10.0f); /* Spread them out along the curb */
    p->position = vec3(start_x, 0.0f, start_z);
}

void penguin_agent_update(penguin_agent_t* p, double dt, light_state_t light) {
    float speed = p->personality.walk_speed * 2.0f;

    switch (p->state) {
        case PENGUIN_WAITING_AT_CURB:
            /* If the light turns green, start crossing */
            if (light == LIGHT_GREEN) {
                p->state = PENGUIN_CROSSING;
            }
            /* Small idle waddle while waiting */
            p->waddle_phase += (float)dt * 2.0f;
            break;

        case PENGUIN_CROSSING:
            /* Move across the road */
            p->position.x += speed * p->direction * (float)dt;

            /* Animate waddle */
            p->waddle_phase += (float)dt * speed * 5.0f;

            /* Check if they reached the other side */
            if ((p->direction == 1 && p->position.x > 10.0f) ||
                (p->direction == -1 && p->position.x < -10.0f)) {
                p->state = PENGUIN_WADDLING_OFF;
                p->position.z += 2.0f; /* Step onto the sidewalk */
            }
            break;

        case PENGUIN_WADDLING_OFF:
            /* Walk away from the crosswalk so they despawn cleanly */
            p->position.x += speed * p->direction * (float)dt;
            p->waddle_phase += (float)dt * speed * 5.0f;

            /* Note: main.c will handle respawning them if they go too far */
            break;
    }
}