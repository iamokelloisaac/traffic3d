/*
* vec4.h — 4-component vector for homogeneous coordinates.
 *
 * Commit: feat(math): add vec4 with from-vec3 and perspective divide
 *
 * Depends on: vec3.h
 * Consumed by: mat4 (vector transform), rasterizer (clip coords).
 */
#ifndef TRAFFIC3D_MATH_VEC4_H
#define TRAFFIC3D_MATH_VEC4_H

#include "traffic3d/math/vec3.h"

typedef struct { float x, y, z, w; } vec4_t;

vec4_t vec4(float x, float y, float z, float w);

/* Lift a vec3 into homogeneous space with a chosen w (typically 1.0). */
vec4_t vec4_from_vec3(vec3_t v, float w);

/* Perspective divide: drop w by dividing xyz by it. Safe against w≈0. */
vec3_t vec4_perspective_divide(vec4_t v);

#endif /* TRAFFIC3D_MATH_VEC4_H */