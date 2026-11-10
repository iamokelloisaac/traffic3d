/*
* lighting.h — flat shading with two directional lights.
 *
 * Commit: feat(render): add flat directional lighting helper
 *
 * Depends on: math/vec3.h, common.h
 * Consumed by: main (during rendering).
 *
 * We use a tiny two-light setup:
 *   - Warm key light from upper-left
 *   - Cool fill light from upper-right, dimmer
 * Plus a global ambient term.
 *
 * Colors come in as 0xRRGGBB; the output is a packed pixel via
 * rgba_from_rgb24 with alpha = 0xFF.
 */
#ifndef TRAFFIC3D_RENDER_LIGHTING_H
#define TRAFFIC3D_RENDER_LIGHTING_H

#include "traffic3d/common.h"
#include "traffic3d/math/vec3.h"

typedef struct {
    vec3_t   direction;     /* pointing FROM the light (unit) */
    uint32_t rgb;           /* 0xRRGGBB */
    float    intensity;     /* 0..1 */
} directional_light_t;

typedef struct {
    directional_light_t key;
    directional_light_t fill;
    float               ambient;   /* 0..1 */
} lighting_t;

/* Sensible defaults: warm key, cool fill, gentle ambient. */
lighting_t lighting_default();

/* Apply shading to a triangle color given its face normal. */
uint32_t lighting_shade(const lighting_t* l, vec3_t normal, uint32_t base_rgb24);

#endif