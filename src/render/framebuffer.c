/*
* framebuffer.c — allocation, clearing, raw writes.
 *
 * Commit: feat(render): implement framebuffer alloc/clear/put
 *
 * Depends on: framebuffer.h, <stdlib.h>, <string.h>
 */
#include "traffic3d/render/framebuffer.h"
#include <stdlib.h>
#include <string.h>

framebuffer_t* framebuffer_create(int width, int height) {
    if (width <= 0 || height <= 0) return NULL;

    framebuffer_t* fb = calloc(1, sizeof *fb);
    if (!fb) return NULL;

    fb->pixels = malloc(sizeof(uint32_t) * (size_t)width * (size_t)height);
    fb->depth  = malloc(sizeof(float)    * (size_t)width * (size_t)height);
    if (!fb->pixels || !fb->depth) {
        free(fb->pixels);
        free(fb->depth);
        free(fb);
        return NULL;
    }
    fb->width  = width;
    fb->height = height;
    framebuffer_clear(fb, rgba_pack(0, 0, 0, 255));
    return fb;
}

void framebuffer_destroy(framebuffer_t* fb) {
    if (!fb) return;
    free(fb->pixels);
    free(fb->depth);
    free(fb);
}

void framebuffer_clear(framebuffer_t* fb, uint32_t color) {
    if (!fb) return;
    size_t n = (size_t)fb->width * (size_t)fb->height;
    for (size_t i = 0; i < n; ++i) fb->pixels[i] = color;
    for (size_t i = 0; i < n; ++i) fb->depth[i]  = 1.0f;
}

void framebuffer_put(framebuffer_t* fb, int x, int y, uint32_t color) {
    if (!fb) return;
    if (x < 0 || y < 0 || x >= fb->width || y >= fb->height) return;
    fb->pixels[(size_t)y * (size_t)fb->width + (size_t)x] = color;
}

void framebuffer_clear_depth(framebuffer_t* fb) {
    if (!fb) return;
    size_t n = (size_t)fb->width * (size_t)fb->height;
    for (size_t i = 0; i < n; ++i) fb->depth[i] = 1.0f;
}

