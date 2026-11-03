/*
* primitives.h — procedural primitive meshes.
 *
 * Commit: feat(geometry): add cube and ground plane factories
 *
 * Depends on: render/mesh.h
 * Consumed by: main, scene setup.
 */
#ifndef TRAFFIC3D_GEOMETRY_PRIMITIVES_H
#define TRAFFIC3D_GEOMETRY_PRIMITIVES_H

#include "traffic3d/render/mesh.h"

/* Unit cube centered at origin, edge length 1, RGB per face. */
mesh_t* primitives_cube(uint32_t rgb24_top,    uint32_t rgb24_bottom,
                        uint32_t rgb24_side);

/* Ground plane on Y = 0, centered at origin, spanning `size` units. */
mesh_t* primitives_plane(float size, uint32_t rgb24);

#endif