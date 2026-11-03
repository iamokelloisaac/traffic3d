/*
* mat4.h — 4x4 column-major matrix.
 *
 * Commit: feat(math): add mat4 with transforms, perspective, look_at, mul
 *
 * Storage: m[col * 4 + row]. Column-major matches OpenGL convention,
 * so mat4_mul(a, b) == a * b applies b to a column vector FIRST, then a.
 * The standard transform pipeline is:
 *
 *     mvp = projection * view * model
 *     clip = mat4_mul_vec4(mvp, object_space_vertex)
 *
 * Depends on: vec3.h, vec4.h
 * Consumed by: camera, scene, main.
 */
#ifndef TRAFFIC3D_MATH_MAT4_H
#define TRAFFIC3D_MATH_MAT4_H

#include "traffic3d/math/vec3.h"
#include "traffic3d/math/vec4.h"

typedef struct { float m[16]; } mat4_t;

/* Identity */
mat4_t mat4_identity(void);

/* Multiplication: result = a * b. Applies b first to a column vector. */
mat4_t mat4_mul(mat4_t a, mat4_t b);

/* Transform a column vector: result = m * v. */
vec4_t mat4_mul_vec4(mat4_t m, vec4_t v);

/* Basic transforms */
mat4_t mat4_translate(vec3_t t);
mat4_t mat4_scale    (vec3_t s);
mat4_t mat4_rotate_x (float radians);
mat4_t mat4_rotate_y (float radians);
mat4_t mat4_rotate_z (float radians);

/* Perspective projection. Produces OpenGL-style NDC (near→-1, far→+1).
 * The rasterizer remaps z to [0,1] before depth-testing.
 * fov_y_rad: vertical field of view.
 * aspect: width / height of the viewport.
 */
mat4_t mat4_perspective(float fov_y_rad, float aspect, float near_z, float far_z);

/* View matrix. Camera at eye looking at target, up is which way is 'up'. */
mat4_t mat4_look_at(vec3_t eye, vec3_t target, vec3_t up);

#endif /* TRAFFIC3D_MATH_MAT4_H */