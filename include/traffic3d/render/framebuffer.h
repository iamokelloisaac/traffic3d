/*
* framebuffer.h — RGBA color buffer + float depth buffer.
 *
 * Commit: feat(render): add framebuffer with color and z-buffer
 *
 * Depends on: common.h
 * Consumed by: rasterizer, terminal, main.
 *
 * The pixel array uses the packed-RGBA layout from common.h.
 * The z-buffer stores [0,1] NDC-remapped depth; 1.0 = far.
 */
#ifndef TRAFFIC3D_RENDER_FRAMEBUFFER_H
#define TRAFFIC3D_RENDER_FRAMEBUFFER_H

#include "traffic3d/common.h"

typedef struct {
    uint32_t* pixels;      /* width * height entries */
    float*    depth;       /* width * height entries */
    int       width;
    int       height;
} framebuffer_t;

/* Allocate buffers. Returns NULL on failure. */
framebuffer_t* framebuffer_create(int width, int height);
void           framebuffer_destroy(framebuffer_t* fb);

/* Wipe colors to a single value and depths to 1.0 (far). */
void framebuffer_clear(framebuffer_t* fb, uint32_t color);

/* Direct pixel write (no depth test). Bounds-checked. */
void framebuffer_put(framebuffer_t* fb, int x, int y, uint32_t color);

/* Reset depth buffer to 1.0 (far) without touching colors.
 * Must be called every frame before rendering. */
void framebuffer_clear_depth(framebuffer_t* fb);

#endif