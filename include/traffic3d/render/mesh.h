/*
* mesh.h — triangle soup with per-face normals and colors.
 *
 * Commit: feat(render): add triangle mesh with dynamic growth
 *
 * Depends on: common.h, math/vec3.h
 * Consumed by: primitives, rasterizer pipeline (via main), agents later.
 */
#ifndef TRAFFIC3D_RENDER_MESH_H
#define TRAFFIC3D_RENDER_MESH_H

#include "traffic3d/common.h"
#include "traffic3d/math/vec3.h"

typedef struct {
    vec3_t   a, b, c;      /* object-space vertices */
    vec3_t   normal;       /* face normal; filled by mesh_finalize */
    uint32_t color;        /* base RGB24 packed via rgba_from_rgb24 */
} triangle_t;

typedef struct {
    triangle_t* tris;
    size_t      count;
    size_t      capacity;
} mesh_t;

mesh_t* mesh_create();
void    mesh_destroy(mesh_t* m);

/* Append one triangle. Color is passed as a 0xRRGGBB value. */
void    mesh_add_triangle(mesh_t* m, vec3_t a, vec3_t b, vec3_t c, uint32_t rgb24);

/* Compute face normals for every triangle. Call after all triangles added. */
void    mesh_finalize(mesh_t* m);

#endif