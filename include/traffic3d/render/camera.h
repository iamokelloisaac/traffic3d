/*
* camera.h — view + projection matrix helper.
 *
 * Commit: feat(render): add camera struct and MVP helpers
 *
 * Depends on: math/vec3.h, math/mat4.h
 * Consumed by: main, scene rendering.
 */
#ifndef TRAFFIC3D_RENDER_CAMERA_H
#define TRAFFIC3D_RENDER_CAMERA_H

#include "traffic3d/math/vec3.h"
#include "traffic3d/math/mat4.h"

typedef struct {
    vec3_t position;
    vec3_t target;
    vec3_t up;
    float  fov_y_radians;
    float  near_z;
    float  far_z;
} camera_t;

camera_t camera_make(vec3_t position, vec3_t target, vec3_t up,
                     float fov_y_radians, float near_z, float far_z);

mat4_t   camera_view(const camera_t* cam);
mat4_t   camera_projection(const camera_t* cam, float aspect);
mat4_t   camera_view_projection(const camera_t* cam, float aspect);

#endif