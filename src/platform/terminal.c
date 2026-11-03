/*
 * terminal.c — notcurses wrapper implementation.
 *
 * Commit: fix(platform): dedicated media plane, real pixel blitting, correct input API
 *
 * Depends on: terminal.h, <notcurses/notcurses.h>, <stdio.h>, <stdlib.h>
 * Consumed by: main (only).
 */
#include "traffic3d/platform/terminal.h"
#include <notcurses/notcurses.h>
#include <stdio.h>
#include <stdlib.h>

static struct notcurses* g_nc = NULL;
static struct ncplane*   g_std = NULL;
static struct ncplane*   g_media = NULL;

bool terminal_init(void) {
    struct notcurses_options opts = {
        .flags = NCOPTION_SUPPRESS_BANNERS,
        .loglevel = NCLOGLEVEL_WARNING,
    };
    g_nc = notcurses_init(&opts, NULL);
    if (!g_nc) return false;

    g_std = notcurses_stdplane(g_nc);

    /*
     * notcurses refuses to blit pixel graphics to the standard plane.
     * We must create a dedicated plane for our framebuffer.
     * We do this ONCE here, so we don't leak memory every frame.
     */
    unsigned rows = 0, cols = 0;
    ncplane_dim_yx(g_std, &rows, &cols);

    struct ncplane_options nopts = {
        .y = 0,
        .x = 0,
        .rows = rows,
        .cols = cols,
    };
    g_media = ncplane_create(g_std, &nopts);
    if (!g_media) return false;

    ncplane_erase(g_std);
    notcurses_render(g_nc);
    return true;
}

void terminal_shutdown(void) {
    if (!g_nc) return;
    ncplane_destroy(g_media);
    notcurses_stop(g_nc);
    g_nc = NULL;
    g_std = NULL;
    g_media = NULL;
}

void terminal_framebuffer_size(int* out_width, int* out_height) {
    if (!g_nc) { *out_width = 320; *out_height = 180; return; }
    unsigned rows = 0, cols = 0;
    ncplane_dim_yx(g_std, &rows, &cols);

    fprintf(stderr, "[terminal] notcurses reports rows=%u cols=%u\n", rows, cols);

    if (cols < 20 || rows < 10) {
        fprintf(stderr, "[terminal] size too small, using fallback 320x180\n");
        *out_width  = 320;
        *out_height = 180;
        return;
    }

    *out_width  = (int)cols;
    *out_height = (int)rows * 2; /* 2 pixels per terminal row (half blocks) */
}

void terminal_present(const framebuffer_t* fb) {
    if (!g_nc || !g_media || !fb) return;

    struct ncvisual* nv = ncvisual_from_rgba(
        fb->pixels,
        fb->height,
        fb->width * (int)sizeof(uint32_t),
        fb->width);
    if (!nv) return;

    struct ncvisual_options vopts = {
        .n       = g_media,          /* Blit to our dedicated plane */
        .scaling = NCSCALE_STRETCH,  /* Stretch to fill the terminal */
        .blitter = NCBLIT_PIXEL,     /* Force kitty true-pixels (smooth edges) */
    };

    /* If the terminal doesn't support pixels, fallback to standard blocks */
    if (!notcurses_check_pixel_support(g_nc)) {
        vopts.blitter = NCBLIT_2x1;
    }

    ncvisual_blit(g_nc, nv, &vopts);
    notcurses_render(g_nc);
    ncvisual_destroy(nv);
}

unsigned terminal_poll_key(void) {
    if (!g_nc) return 0;

    ncinput ni = {0};
    uint32_t id = notcurses_get_nblock(g_nc, &ni);

    if (id == 0) return 0;
    if (id == (uint32_t)-1) return 0;
    if (id == NCKEY_RESIZE) return 0;

    if (ni.utf8[0]) return (unsigned)(unsigned char)ni.utf8[0];
    return (unsigned)id;
}