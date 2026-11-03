/*
* scene.c — fixed-capacity object array.
 *
 * Commit: feat(scene): implement scene_init/clear/add
 *
 * Depends on: scene.h
 */
#include "traffic3d/scene/scene.h"

void scene_init(scene_t* s) { s->count = 0; }
void scene_clear(scene_t* s) { s->count = 0; }

object_t* scene_add(scene_t* s, object_t obj) {
    if (s->count >= TRAFFIC3D_MAX_OBJECTS) return NULL;
    s->objects[s->count] = obj;
    return &s->objects[s->count++];
}