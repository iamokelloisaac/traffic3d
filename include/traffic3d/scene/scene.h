/*
* scene.h — flat list of renderable objects.
 *
 * Commit: feat(scene): add scene container with add/clear
 *
 * Depends on: scene/object.h
 * Consumed by: main, agents (add car/penguin objects later).
 */
#ifndef TRAFFIC3D_SCENE_SCENE_H
#define TRAFFIC3D_SCENE_SCENE_H

#include "traffic3d/scene/object.h"
#include "traffic3d/common.h"

typedef struct {
    object_t objects[TRAFFIC3D_MAX_OBJECTS];
    size_t   count;
} scene_t;

void      scene_init(scene_t* s);
void      scene_clear(scene_t* s);
object_t* scene_add(scene_t* s, object_t obj);   /* returns ptr or NULL */

#endif