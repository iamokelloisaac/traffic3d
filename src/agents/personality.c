//
// Created by anonymous on 11/3/26.
//
/*
 * personality.c — Implementation of procedural penguin generation.
 *
 * Commit: feat(agents): implement seeded personality generation
 *
 * Depends on: personality.h, <stdlib.h>
 */
#include "traffic3d/agents/personality.h"
#include <stdlib.h>

personality_t personality_generate(uint32_t seed, const config_t* cfg) {
    personality_t p;

    /* Simple LCG for deterministic pseudo-randomness based on the seed */
    seed = seed * 1103515245 + 12345;
    float r1 = (float)((seed >> 16) & 0x7FFF) / 32767.0f;

    seed = seed * 1103515245 + 12345;
    float r2 = (float)((seed >> 16) & 0x7FFF) / 32767.0f;

    seed = seed * 1103515245 + 12345;
    float r3 = (float)((seed >> 16) & 0x7FFF) / 32767.0f;

    p.size = 0.8f + (r1 * 0.4f);
    p.walk_speed = cfg->penguin_speed_min + (r2 * (cfg->penguin_speed_max - cfg->penguin_speed_min));
    p.waddle_amplitude = 0.1f + (r3 * 0.2f);

    /* 10% chance of a navy tint, else black */
    p.body_color = (r1 > 0.9f) ? 0x1A1A2E : 0x111111;

    /* 20% chance of a cream belly, else white */
    p.belly_color = (r2 > 0.8f) ? 0xFFFDD0 : 0xFFFFFF;

    /* 5% chance of accessory */
    p.has_accessory = (r3 < cfg->penguin_accessory_chance);

    return p;
}