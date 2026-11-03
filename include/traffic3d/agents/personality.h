/*
* personality.h — Procedural generation of unique penguin traits.
 *
 * Commit: feat(agents): add personality generator for unique penguins
 *
 * Depends on: common.h, config.h
 * Consumed by: penguin_agent.c, main.c
 */
#ifndef TRAFFIC3D_AGENTS_PERSONALITY_H
#define TRAFFIC3D_AGENTS_PERSONALITY_H

#include "traffic3d/common.h"
#include "traffic3d/config.h"

typedef struct {
    float size;             /* 0.8 to 1.2 */
    float walk_speed;       /* speed multiplier */
    float waddle_amplitude; /* how much they rock side-to-side */
    uint32_t body_color;    /* mostly black, sometimes navy */
    uint32_t belly_color;   /* white or cream */
    bool has_accessory;     /* scarf or hat (not fully implemented visually yet) */
} personality_t;

/* Generate a personality from a seed (use time or an incrementing counter) */
personality_t personality_generate(uint32_t seed, const config_t* cfg);

#endif