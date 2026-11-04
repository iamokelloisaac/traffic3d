/*
* penguin_agent.c — Implementation of penguin crossing behavior.
 *
 * Commit: fix(agents): restrict crossing to exact solid walk phase
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

    float start_x = (direction == 1) ? -10.0f : 10.0f;
    float start_z = 10.5f + ((float)(seed % 30) / 10.0f);
    p->position = vec3(start_x, 0.0f, start_z);
}

void penguin_agent_update(penguin_agent_t* p, double dt, bool is_walk_phase) {
    float speed = p->personality.walk_speed * 2.0f;

    switch (p->state) {
        case PENGUIN_WAITING_AT_CURB:
            /* Only start crossing if it is the exact solid walk phase */
            if (is_walk_phase) {
                p->state = PENGUIN_CROSSING;
            }
            p->waddle_phase += (float)dt * 2.0f;
            break;

        case PENGUIN_CROSSING:
            p->position.x += speed * p->direction * (float)dt;
            p->waddle_phase += (float)dt * speed * 5.0f;

            if ((p->direction == 1 && p->position.x > 10.0f) ||
                (p->direction == -1 && p->position.x < -10.0f)) {
                p->state = PENGUIN_WADDLING_OFF;
                p->position.z += 2.0f;
                }
            break;

        case PENGUIN_WADDLING_OFF:
            p->position.x += speed * p->direction * (float)dt;
            p->waddle_phase += (float)dt * speed * 5.0f;
            break;
    }
}