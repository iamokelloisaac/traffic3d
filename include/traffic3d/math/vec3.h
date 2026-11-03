/*
* vec3.h — 3-component single-precision vector.
 *
 * Commit: feat(math): add vec3 with add/sub/scale/dot/cross/length/normalize
 *
 * Depends on: <math.h> (for sqrtf)
 * Consumed by: vec4, mat4, mesh, rasterizer, camera, lighting, agents.
 */
#ifndef TRAFFIC3D_MATH_VEC3_H
#define TRAFFIC3D_MATH_VEC3_H

typedef struct { float x, y, z; } vec3_t;

/* Constructors */
vec3_t vec3(float x, float y, float z);
vec3_t vec3_zero(void);

/* Arithmetic */
vec3_t vec3_add   (vec3_t a, vec3_t b);
vec3_t vec3_sub   (vec3_t a, vec3_t b);
vec3_t vec3_scale (vec3_t v, float s);
vec3_t vec3_mul   (vec3_t a, vec3_t b);   /* component-wise */

/* Products and lengths */
float  vec3_dot        (vec3_t a, vec3_t b);
vec3_t vec3_cross      (vec3_t a, vec3_t b);
float  vec3_length_sq  (vec3_t v);
float  vec3_length     (vec3_t v);

/* Returns the zero vector if v is zero-length (safe normalize). */
vec3_t vec3_normalize(vec3_t v);

#endif /* TRAFFIC3D_MATH_VEC3_H */