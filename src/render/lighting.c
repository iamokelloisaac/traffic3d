/*
 * lighting.c — flat directional shading implementation.
 *
 * Commit: feat(render): implement Lambert directional lighting
 *
 * Depends on: lighting.h, math/mat4.h (not needed here), <math.h>
 */
#include "traffic3d/render/lighting.h"
#include <math.h>

lighting_t lighting_default(void) {
    return (lighting_t){
        .key     = { .direction = vec3_normalize(vec3(-0.5f, -1.0f, -0.3f)),
                     .rgb = 0xFFF2D0, .intensity = 0.85f },
        .fill    = { .direction = vec3_normalize(vec3( 0.6f, -0.4f,  0.4f)),
                     .rgb = 0xA0C8FF, .intensity = 0.35f },
        .ambient = 0.18f,
    };
}

uint32_t lighting_shade(const lighting_t* l, vec3_t normal, uint32_t base_rgb24) {
    float br = (float)((base_rgb24 >> 16) & 0xFF) / 255.0f;
    float bg = (float)((base_rgb24 >>  8) & 0xFF) / 255.0f;
    float bb = (float)( base_rgb24        & 0xFF) / 255.0f;

    /* Key */
    float dk = fmaxf(0.0f, -vec3_dot(normal, l->key.direction));
    float kr = (float)((l->key.rgb >> 16) & 0xFF) / 255.0f;
    float kg = (float)((l->key.rgb >>  8) & 0xFF) / 255.0f;
    float kb = (float)( l->key.rgb        & 0xFF) / 255.0f;

    /* Fill */
    float df = fmaxf(0.0f, -vec3_dot(normal, l->fill.direction));
    float fr = (float)((l->fill.rgb >> 16) & 0xFF) / 255.0f;
    float fg = (float)((l->fill.rgb >>  8) & 0xFF) / 255.0f;
    float fb = (float)( l->fill.rgb        & 0xFF) / 255.0f;

    float r = br * (l->ambient
                  + dk * l->key.intensity  * kr
                  + df * l->fill.intensity * fr);
    float g = bg * (l->ambient
                  + dk * l->key.intensity  * kg
                  + df * l->fill.intensity * fg);
    float b = bb * (l->ambient
                  + dk * l->key.intensity  * kb
                  + df * l->fill.intensity * fb);

    /* Clamp + pack. */
    int ri = (int)(fminf(1.0f, r) * 255.0f);
    int gi = (int)(fminf(1.0f, g) * 255.0f);
    int bi = (int)(fminf(1.0f, b) * 255.0f);
    return rgba_pack((uint8_t)ri, (uint8_t)gi, (uint8_t)bi, 0xFF);
}