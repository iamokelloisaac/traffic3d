/*
* rasterizer.h — software triangle rasterizer with z-buffer.
 *
 * Commit: feat(render): add barycentric rasterizer header
 *
 * Depends on: framebuffer.h
 * Consumed by: main (per-frame render loop).
 */
#ifndef TRAFFIC3D_RENDER_RASTERIZER_H
#define TRAFFIC3D_RENDER_RASTERIZER_H

#include "traffic3d/render/framebuffer.h"

/*
 * A triangle's vertex after transform + perspective divide + viewport map.
 *   x, y : screen pixel coordinates (origin top-left, +y down)
 *   z    : [0,1] depth, 0 = near, 1 = far
 */
typedef struct {
    float x, y, z;
} screen_vertex_t;

/*
 * Fill a triangle with a flat color, using a depth test per pixel.
 * Vertices may be in any winding order.
 * Frustum-clipping is not performed; callers should pass triangles that
 * are already inside the view volume (or accept back-face artifacts).
 */
void rasterizer_triangle(framebuffer_t* fb,
                         screen_vertex_t v0,
                         screen_vertex_t v1,
                         screen_vertex_t v2,
                         uint32_t color);

#endif