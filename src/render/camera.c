/*
* camera.c — camera helpers wrapping mat4 look_at / perspective.
 *
 * Commit: feat(render): implement camera helpers
 *
 * Depends on: camera.h
 */
#include "traffic3d/render/camera.h"

camera_t camera_make(vec3_t position, vec3_t target, vec3_t up,
                     float fov_y_radians, float near_z, float far_z) {
    return (camera_t){
        .position      = position,
        .target        = target,
        .up            = up,
        .fov_y_radians = fov_y_radians,
        .near_z        = near_z,
        .far_z         = far_z,
    };
}

mat4_t camera_view(const camera_t* cam) {
    return mat4_look_at(cam->position, cam->target, cam->up);
}

mat4_t camera_projection(const camera_t* cam, float aspect) {
    return mat4_perspective(cam->fov_y_radians, aspect, cam->near_z, cam->far_z);
}

mat4_t camera_view_projection(const camera_t* cam, float aspect) {
    return mat4_mul(camera_projection(cam, aspect), camera_view(cam));
}