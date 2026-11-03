/*
* vec4.c — implementation of vec4_t.
 *
 * Commit: feat(math): implement vec4 operations
 *
 * Depends on: vec4.h
 */
#include "traffic3d/math/vec4.h"

vec4_t vec4(float x, float y, float z, float w) { return (vec4_t){ x, y, z, w }; }

vec4_t vec4_from_vec3(vec3_t v, float w) { return (vec4_t){ v.x, v.y, v.z, w }; }

vec3_t vec4_perspective_divide(vec4_t v) {
    if (v.w <= 0.0f) return (vec3_t){ 0.0f, 0.0f, 0.0f };
    float inv = 1.0f / v.w;
    return (vec3_t){ v.x * inv, v.y * inv, v.z * inv };
}