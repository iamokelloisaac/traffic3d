//
// Created by anonymous on 11/3/26.
//
/*
 * object.c — trivial object constructors.
 *
 * Commit: feat(scene): implement object_make
 *
 * Depends on: object.h
 * Consumed by: scene, main.
 *
 * This file is intentionally tiny. An object is just:
 *   - a pointer to a mesh (geometry),
 *   - a transform (position, rotation, scale),
 *   - an optional color tint,
 *   - a visibility flag.
 *
 * The renderer reads these fields each frame. Nothing here does any
 * work at runtime; it's purely a constructor. That's the point —
 * "object" is a data record, not a behaviour.
 */
#include "traffic3d/scene/object.h"

object_t object_make(mesh_t* mesh, vec3_t pos, vec3_t rot, vec3_t scale) {
    return (object_t){
        .mesh       = mesh,
        .position   = pos,
        .rotation   = rot,
        .scale      = scale,
        .color_tint = 0xFFFFFF,   /* no tint by default */
        .visible    = true,
    };
}