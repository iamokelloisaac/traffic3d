/*
 * rasterizer.c — barycentric triangle fill with depth test.
 *
 * Commit: feat(render): implement software rasterizer
 *
 * Depends on: rasterizer.h, <math.h>
 *
 * Algorithm:
 *   1. Compute signed triangle area (twice the actual area).
 *   2. Bounding box clamp.
 *   3. For each pixel in the box, compute three barycentric edge weights.
 *   4. Accept when all weights share the area's sign (i.e. inside).
 *   5. Interpolate depth; test against z-buffer; write on pass.
 *
 * We intentionally skip perspective-correct interpolation because the
 * scene is flat-shaded and low-poly. If Gouraud shading arrives later,
 * we extend the API with per-vertex colors and 1/w.
 */
#include "traffic3d/render/rasterizer.h"
#include <math.h>

static inline float edge_fn(float ax, float ay, float bx, float by,
                            float px, float py) {
    return (px - ax) * (by - ay) - (py - ay) * (bx - ax);
}

void rasterizer_triangle(framebuffer_t* fb,
                         screen_vertex_t v0,
                         screen_vertex_t v1,
                         screen_vertex_t v2,
                         uint32_t color) {
    if (!fb) return;

    /* Signed double-area. Small area means degenerate → skip. */
    /* Signed double-area. Small area means degenerate → skip. */
    /* NOTE: This must match the sign of the edge_fn calculations below. */
    float area = (v2.x - v0.x) * (v1.y - v0.y)
               - (v1.x - v0.x) * (v2.y - v0.y);
    if (fabsf(area) < 1e-6f) return;

    /* Bounding box clamped to the framebuffer. */
    int min_x = (int)floorf(fminf(v0.x, fminf(v1.x, v2.x)));
    int max_x = (int)ceilf (fmaxf(v0.x, fmaxf(v1.x, v2.x)));
    int min_y = (int)floorf(fminf(v0.y, fminf(v1.y, v2.y)));
    int max_y = (int)ceilf (fmaxf(v0.y, fmaxf(v1.y, v2.y)));
    if (min_x < 0) min_x = 0;
    if (min_y < 0) min_y = 0;
    if (max_x >= fb->width)  max_x = fb->width  - 1;
    if (max_y >= fb->height) max_y = fb->height - 1;
    if (min_x > max_x || min_y > max_y) return;

    float inv_area = 1.0f / area;
    int positive = (area > 0.0f);

    for (int y = min_y; y <= max_y; ++y) {
        for (int x = min_x; x <= max_x; ++x) {
            float px = (float)x + 0.5f;
            float py = (float)y + 0.5f;

            /* Sub-triangle areas (three edge functions). */
            float w0 = edge_fn(v1.x, v1.y, v2.x, v2.y, px, py);
            float w1 = edge_fn(v2.x, v2.y, v0.x, v0.y, px, py);
            float w2 = edge_fn(v0.x, v0.y, v1.x, v1.y, px, py);

            int in = positive
                ? (w0 >= 0.0f && w1 >= 0.0f && w2 >= 0.0f)
                : (w0 <= 0.0f && w1 <= 0.0f && w2 <= 0.0f);
            if (!in) continue;

            /* Depth interpolation (linear in screen space). */
            float depth = (w0 * v0.z + w1 * v1.z + w2 * v2.z) * inv_area;
            if (depth < 0.0f) depth = 0.0f;
            if (depth > 1.0f) depth = 1.0f;

            size_t idx = (size_t)y * (size_t)fb->width + (size_t)x;
            if (depth < fb->depth[idx]) {
                fb->depth[idx]  = depth;
                fb->pixels[idx] = color;
            }
        }
    }
}