/*
* terminal.h — notcurses wrapper: init, present, poll-key, shutdown.
 *
 * Commit: feat(platform): add notcurses terminal adapter
 *
 * Depends on: render/framebuffer.h
 * Consumed by: main.
 *
 * This is the ONLY translation unit that may include <notcurses/notcurses.h>.
 * Everything above this layer stays portable.
 *
 * Sizing contract:
 *   terminal_init() queries the terminal and exposes the framebuffer
 *   size that best matches it. Callers should then allocate a framebuffer
 *   of that size and pass it to terminal_present() each frame.
 *
 *   For a terminal of (rows, cols) cells we allocate a framebuffer of
 *   cols x (rows * 2) pixels — one pixel per column, two pixels per row
 *   (matching the ▀/▄ half-block blitter). Character cells are ~1:2
 *   wide:tall, so pixels come out roughly square.
 */
#ifndef TRAFFIC3D_PLATFORM_TERMINAL_H
#define TRAFFIC3D_PLATFORM_TERMINAL_H

#include "traffic3d/render/framebuffer.h"

bool terminal_init();
void terminal_shutdown();

/* Framebuffer dimensions recommended for this terminal. */
void terminal_framebuffer_size(int* out_width, int* out_height);

/* Push the framebuffer to the terminal (blit + render). */
void terminal_present(const framebuffer_t* fb);

/* Poll for a key. Returns 0 if none, otherwise the Unicode codepoint. */
unsigned terminal_poll_key();

#endif