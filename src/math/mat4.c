/*
 * mat4.c — implementation of mat4_t.
 *
 * Commit: feat(math): implement mat4 operations
 *
 * Depends on: mat4.h, <math.h>
 */
#include "traffic3d/math/mat4.h"
#include <math.h>

mat4_t mat4_identity(void) {
    mat4_t r = { .m = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1,
    }};
    return r;
}

/*
 * Column-major multiply: result[col][row] = sum_k a[k][row] * b[col][k].
 * In flat index:  a[k*4 + row] * b[col*4 + k]
 */
mat4_t mat4_mul(mat4_t a, mat4_t b) {
    mat4_t r = {0};
    for (int col = 0; col < 4; ++col) {
        for (int row = 0; row < 4; ++row) {
            float s = 0.0f;
            for (int k = 0; k < 4; ++k) {
                s += a.m[k * 4 + row] * b.m[col * 4 + k];
            }
            r.m[col * 4 + row] = s;
        }
    }
    return r;
}

vec4_t mat4_mul_vec4(mat4_t m, vec4_t v) {
    vec4_t r;
    r.x = m.m[0]*v.x + m.m[4]*v.y + m.m[ 8]*v.z + m.m[12]*v.w;
    r.y = m.m[1]*v.x + m.m[5]*v.y + m.m[ 9]*v.z + m.m[13]*v.w;
    r.z = m.m[2]*v.x + m.m[6]*v.y + m.m[10]*v.z + m.m[14]*v.w;
    r.w = m.m[3]*v.x + m.m[7]*v.y + m.m[11]*v.z + m.m[15]*v.w;
    return r;
}

mat4_t mat4_translate(vec3_t t) {
    mat4_t r = mat4_identity();
    r.m[12] = t.x;
    r.m[13] = t.y;
    r.m[14] = t.z;
    return r;
}

mat4_t mat4_scale(vec3_t s) {
    mat4_t r = mat4_identity();
    r.m[0]  = s.x;
    r.m[5]  = s.y;
    r.m[10] = s.z;
    return r;
}

mat4_t mat4_rotate_x(float a) {
    float c = cosf(a), s = sinf(a);
    mat4_t r = mat4_identity();
    r.m[5]  =  c; r.m[6]  = s;
    r.m[9]  = -s; r.m[10] = c;
    return r;
}

mat4_t mat4_rotate_y(float a) {
    float c = cosf(a), s = sinf(a);
    mat4_t r = mat4_identity();
    r.m[0]  =  c; r.m[2]  = -s;
    r.m[8]  =  s; r.m[10] =  c;
    return r;
}

mat4_t mat4_rotate_z(float a) {
    float c = cosf(a), s = sinf(a);
    mat4_t r = mat4_identity();
    r.m[0] = c; r.m[1] = s;
    r.m[4] = -s; r.m[5] = c;
    return r;
}

/*
 * OpenGL-style perspective. Maps z from [near, far] → [-1, +1].
 * The rasterizer will remap that to [0,1] before the depth test.
 */
mat4_t mat4_perspective(float fov_y_rad, float aspect, float near_z, float far_z) {
    float f = 1.0f / tanf(fov_y_rad * 0.5f);
    mat4_t r = {0};
    r.m[0]  = f / aspect;
    r.m[5]  = f;
    r.m[10] = (far_z + near_z) / (near_z - far_z);
    r.m[11] = -1.0f;
    r.m[14] = (2.0f * far_z * near_z) / (near_z - far_z);
    return r;
}

mat4_t mat4_look_at(vec3_t eye, vec3_t target, vec3_t up) {
    /* Forward is from eye → target (opposite of typical 'camera looks -z'). */
    vec3_t f = vec3_normalize(vec3_sub(target, eye));
    vec3_t s = vec3_normalize(vec3_cross(f, up));
    vec3_t u = vec3_cross(s, f);

    mat4_t r = mat4_identity();
    r.m[0] =  s.x; r.m[4] =  s.y; r.m[ 8] =  s.z;
    r.m[1] =  u.x; r.m[5] =  u.y; r.m[ 9] =  u.z;
    r.m[2] = -f.x; r.m[6] = -f.y; r.m[10] = -f.z;
    r.m[12] = -vec3_dot(s, eye);
    r.m[13] = -vec3_dot(u, eye);
    r.m[14] =  vec3_dot(f, eye);
    return r;
}