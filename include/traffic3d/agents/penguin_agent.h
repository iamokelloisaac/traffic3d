/*
* penguin_agent.h — Pedestrians moving across the crosswalk.
 *
 * Commit: feat(agents): add penguin agent header with state machine
 *
 * Depends on: personality.h, traffic/state_machine.h
 * Consumed by: main.c
 */
#ifndef TRAFFIC3D_AGENTS_PENGUIN_AGENT_H
#define TRAFFIC3D_AGENTS_PENGUIN_AGENT_H

#include "traffic3d/agents/personality.h"
#include "traffic3d/traffic/state_machine.h"
#include "traffic3d/math/vec3.h"

typedef enum {
    PENGUIN_WAITING_AT_CURB,
    PENGUIN_CROSSING,
    PENGUIN_WADDLING_OFF
} penguin_state_t;

typedef struct {
    vec3_t position;
    penguin_state_t state;
    personality_t personality;
    float waddle_phase;
    int direction;      /* 1 for left-to-right, -1 for right-to-left */
    size_t object_index;
} penguin_agent_t;

void penguin_agent_init(penguin_agent_t* p, int direction, size_t obj_idx, const config_t* cfg, uint32_t seed);
void penguin_agent_update(penguin_agent_t* p, double dt, bool is_walk_phase);
#endif