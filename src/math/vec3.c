/*
* vec3.c — implementation of vec3_t.
 *
 * Commit: feat(math): implement vec3 operations
 *
 * Depends on: vec3.h
 */
#include "traffic3d/math/vec3.h"
#include <math.h>

vec3_t vec3(float x, float y, float z)  { return (vec3_t){ x, y, z }; }
vec3_t vec3_zero(void)                  { return (vec3_t){ 0.0f, 0.0f, 0.0f }; }

vec3_t vec3_add(vec3_t a, vec3_t b) {
    return (vec3_t){ a.x + b.x, a.y + b.y, a.z + b.z };
}
vec3_t vec3_sub(vec3_t a, vec3_t b) {
    return (vec3_t){ a.x - b.x, a.y - b.y, a.z - b.z };
}
vec3_t vec3_scale(vec3_t v, float s) {
    return (vec3_t){ v.x * s, v.y * s, v.z * s };
}
vec3_t vec3_mul(vec3_t a, vec3_t b) {
    return (vec3_t){ a.x * b.x, a.y * b.y, a.z * b.z };
}

float vec3_dot(vec3_t a, vec3_t b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

vec3_t vec3_cross(vec3_t a, vec3_t b) {
    return (vec3_t){
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x,
    };
}

float vec3_length_sq(vec3_t v) { return vec3_dot(v, v); }
float vec3_length(vec3_t v)    { return sqrtf(vec3_length_sq(v)); }

vec3_t vec3_normalize(vec3_t v) {
    float len = vec3_length(v);
    if (len < 1e-8f) return vec3_zero();
    float inv = 1.0f / len;
    return (vec3_t){ v.x * inv, v.y * inv, v.z * inv };
}