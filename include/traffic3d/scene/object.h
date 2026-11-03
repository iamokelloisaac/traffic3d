/*
* object.h — one renderable instance in the scene.
 *
 * Commit: feat(scene): add object struct with transform and mesh
 *
 * Depends on: render/mesh.h, math/vec3.h
 * Consumed by: scene, main.
 */
#ifndef TRAFFIC3D_SCENE_OBJECT_H
#define TRAFFIC3D_SCENE_OBJECT_H

#include "traffic3d/render/mesh.h"
#include "traffic3d/math/vec3.h"

typedef struct {
    mesh_t*  mesh;
    vec3_t   position;
    vec3_t   rotation;      /* radians, applied as Rz * Ry * Rx */
    vec3_t   scale;
    uint32_t color_tint;    /* multiplied into mesh colors; 0xFFFFFF = none */
    bool     visible;
} object_t;

object_t object_make(mesh_t* mesh, vec3_t pos, vec3_t rot, vec3_t scale);

#endif