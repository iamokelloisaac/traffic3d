/*
 * common.h — shared types, macros, and inline helpers.
 *
 * Commit: feat(core): add common header with color packing, light states, limits
 *
 * Depends on: <stdint.h>, <stdbool.h>, <stddef.h>
 * Consumed by: literally everything else.
 *
 * This header must remain dependency-free of any other project header
 * so it can be included anywhere in the tree without cycles.
 */
#ifndef TRAFFIC3D_COMMON_H
#define TRAFFIC3D_COMMON_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* ─── Framebuffer pixel format ──────────────────────────────────
 *
 * A framebuffer pixel is uint32_t storing RGBA in ascending byte
 * order (R at offset 0, A at offset 3). On little-endian hardware
 * that's numerically 0xAABBGGRR — but we never rely on that;
 * the helpers below are the ONLY way to build a pixel.
 *
 * We use this layout because notcurses' ncvisual_from_rgba()
 * reads bytes in R,G,B,A order.
 */
static inline uint32_t rgba_pack(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return ((uint32_t)a << 24) | ((uint32_t)b << 16)
         | ((uint32_t)g <<  8) | ((uint32_t)r);
}

/* Build a pixel from a 0xRRGGBB literal with a chosen alpha. */
static inline uint32_t rgba_from_rgb24(uint32_t rgb, uint8_t a) {
    return rgba_pack((rgb >> 16) & 0xFF, (rgb >> 8) & 0xFF, rgb & 0xFF, a);
}

/* Accessors — used by the rasterizer when it needs components. */
static inline uint8_t rgba_r(uint32_t p) { return (uint8_t)( p        & 0xFF); }
static inline uint8_t rgba_g(uint32_t p) { return (uint8_t)((p >>  8) & 0xFF); }
static inline uint8_t rgba_b(uint32_t p) { return (uint8_t)((p >> 16) & 0xFF); }
static inline uint8_t rgba_a(uint32_t p) { return (uint8_t)((p >> 24) & 0xFF); }

/* ─── Traffic light states ──────────────────────────────────────
 *
 * The car symbol is tristate (red/yellow/green); the pedestrian
 * symbol is bistate (red/green). We store both using this enum
 * for uniformity — pedestrian simply never uses LIGHT_YELLOW.
 */
typedef enum {
    LIGHT_RED    = 0,
    LIGHT_YELLOW = 1,
    LIGHT_GREEN  = 2,
} light_state_t;

/* ─── Global limits ─────────────────────────────────────────────
 *
 * Compile-time caps so scene/object arrays can be fixed-size
 * without malloc. Tune later if the population needs to grow.
 */
#define TRAFFIC3D_MAX_OBJECTS            512
#define TRAFFIC3D_MAX_TRIANGLES_PER_MESH 4096

#endif /* TRAFFIC3D_COMMON_H */