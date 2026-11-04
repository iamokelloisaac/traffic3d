/*
 * main.c — Entry point. Phase 6: Final HUD polish with pixel-art icons.
 *
 * Commit: fix(main): render 3D scene before HUD to prevent ground occlusion
 *
 * Depends on: everything.
 */
#include "traffic3d/common.h"
#include "traffic3d/config.h"
#include "traffic3d/platform/terminal.h"
#include "traffic3d/platform/timing.h"
#include "traffic3d/math/vec3.h"
#include "traffic3d/math/mat4.h"
#include "traffic3d/render/framebuffer.h"
#include "traffic3d/render/mesh.h"
#include "traffic3d/render/rasterizer.h"
#include "traffic3d/render/camera.h"
#include "traffic3d/render/lighting.h"
#include "traffic3d/scene/object.h"
#include "traffic3d/scene/scene.h"
#include "traffic3d/geometry/primitives.h"
#include "traffic3d/geometry/road_mesh.h"
#include "traffic3d/geometry/light_mesh.h"
#include "traffic3d/traffic/state_machine.h"
#include "traffic3d/agents/car_agent.h"
#include "traffic3d/agents/penguin_agent.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/* ── 5x7 Pixel Font for the Text HUD ─────────────────────────── */
static const uint8_t FONT[][5] = {
    {0x00,0x00,0x00,0x00,0x00}, /* Space */
    {0x7C,0x12,0x11,0x12,0x7C}, /* A */
    {0x7F,0x49,0x49,0x49,0x36}, /* B */
    {0x3E,0x41,0x41,0x41,0x22}, /* C */
    {0x7F,0x41,0x41,0x22,0x1C}, /* D */
    {0x7F,0x49,0x49,0x49,0x41}, /* E */
    {0x7F,0x09,0x09,0x09,0x01}, /* F */
    {0x3E,0x41,0x49,0x49,0x7A}, /* G */
    {0x7F,0x08,0x08,0x08,0x7F}, /* H */
    {0x00,0x41,0x7F,0x41,0x00}, /* I */
    {0x20,0x40,0x41,0x3F,0x01}, /* J */
    {0x7F,0x08,0x14,0x22,0x41}, /* K */
    {0x7F,0x40,0x40,0x40,0x40}, /* L */
    {0x7F,0x02,0x0C,0x02,0x7F}, /* M */
    {0x7F,0x04,0x08,0x10,0x7F}, /* N */
    {0x3E,0x41,0x41,0x41,0x3E}, /* O */
    {0x7F,0x09,0x09,0x09,0x06}, /* P */
    {0x3E,0x41,0x51,0x21,0x5E}, /* Q */
    {0x7F,0x09,0x19,0x29,0x46}, /* R */
    {0x46,0x49,0x49,0x49,0x31}, /* S */
    {0x01,0x01,0x7F,0x01,0x01}, /* T */
    {0x3F,0x40,0x40,0x40,0x3F}, /* U */
    {0x1F,0x20,0x40,0x20,0x1F}, /* V */
    {0x3F,0x40,0x38,0x40,0x3F}, /* W */
    {0x63,0x14,0x08,0x14,0x63}, /* X */
    {0x07,0x08,0x70,0x08,0x07}, /* Y */
    {0x61,0x51,0x49,0x45,0x43}  /* Z */
};

/* ── 12x12 Pixel Icons for the Giant HUD ────────────────────── */
static const char* ICON_CAR[] = {
    "............",
    "...XXXXXX...",
    "..XXXXXXXX..",
    ".XXXXXXXXXX.",
    "XXXXXXXXXXXX",
    "XXXXXXXXXXXX",
    "XX.XXXXXX.XX",
    "XX.XXXXXX.XX",
    "............",
    "............",
    "............",
    "............"
};

static const char* ICON_PENGUIN[] = {
    "....XXXX....",
    "...XXXXXX...",
    "..XXXXXXXX..",
    "..XXXXXXXX..",
    "..XXXXXXXX..",
    "...XXXXXX...",
    "...XXXXXX...",
    "...XXXXXX...",
    "...XX..XX...",
    "...XX..XX...",
    "............",
    "............"
};

static void draw_text(framebuffer_t* fb, int x, int y, const char* text, uint32_t color, int scale) {
    int cursor_x = x;
    for (int i = 0; text[i] != '\0'; i++) {
        char c = text[i];
        int idx = -1;
        if (c >= 'A' && c <= 'Z') idx = (c - 'A') + 1;
        else if (c == ' ') idx = 0;

        if (idx >= 0) {
            for (int col = 0; col < 5; col++) {
                uint8_t bits = FONT[idx][col];
                for (int row = 0; row < 7; row++) {
                    if (bits & (1 << row)) {
                        for (int sy = 0; sy < scale; sy++) {
                            for (int sx = 0; sx < scale; sx++) {
                                framebuffer_put(fb, cursor_x + col * scale + sx, y + row * scale + sy, color);
                            }
                        }
                    }
                }
            }
        }
        cursor_x += 6 * scale;
    }
}

static void draw_icon(framebuffer_t* fb, int x, int y, const char** icon, uint32_t color, int scale) {
    for (int row = 0; row < 12; row++) {
        for (int col = 0; col < 12; col++) {
            if (icon[row][col] == 'X') {
                for (int sy = 0; sy < scale; sy++) {
                    for (int sx = 0; sx < scale; sx++) {
                        framebuffer_put(fb, x + col * scale + sx, y + row * scale + sy, color);
                    }
                }
            }
        }
    }
}

static void draw_rect(framebuffer_t* fb, int x, int y, int w, int h, uint32_t color) {
    for (int j = 0; j < h; j++) {
        for (int i = 0; i < w; i++) {
            framebuffer_put(fb, x + i, y + j, color);
        }
    }
}

/* ── Procedural Meshes ────────────────────────────────────────── */
static mesh_t* create_car_mesh(uint32_t color) {
    mesh_t* m = mesh_create();
    vec3_t b = vec3(0,0,0), size = vec3(1.8f, 0.6f, 3.8f);
    vec3_t v000 = vec3(b.x-size.x/2, b.y-size.y/2, b.z-size.z/2), v001 = vec3(b.x-size.x/2, b.y-size.y/2, b.z+size.z/2), v010 = vec3(b.x-size.x/2, b.y+size.y/2, b.z-size.z/2), v011 = vec3(b.x-size.x/2, b.y+size.y/2, b.z+size.z/2);
    vec3_t v100 = vec3(b.x+size.x/2, b.y-size.y/2, b.z-size.z/2), v101 = vec3(b.x+size.x/2, b.y-size.y/2, b.z+size.z/2), v110 = vec3(b.x+size.x/2, b.y+size.y/2, b.z-size.z/2), v111 = vec3(b.x+size.x/2, b.y+size.y/2, b.z+size.z/2);
    mesh_add_triangle(m, v001, v011, v111, color); mesh_add_triangle(m, v001, v111, v101, color); mesh_add_triangle(m, v100, v110, v010, color); mesh_add_triangle(m, v100, v010, v000, color);
    mesh_add_triangle(m, v010, v110, v111, color); mesh_add_triangle(m, v010, v111, v011, color); mesh_add_triangle(m, v000, v001, v101, color); mesh_add_triangle(m, v000, v101, v100, color);
    mesh_add_triangle(m, v000, v010, v011, color); mesh_add_triangle(m, v000, v011, v001, color); mesh_add_triangle(m, v101, v111, v110, color); mesh_add_triangle(m, v101, v110, v100, color);
    float cx = 1.4f, cy = 0.5f, cz = 2.0f;
    vec3_t r000 = vec3(b.x-cx/2, b.y+size.y/2, b.z-cz/2), r001 = vec3(b.x-cx/2, b.y+size.y/2, b.z+cz/2), r010 = vec3(b.x-cx/2, b.y+size.y/2+cy, b.z-cz/2), r011 = vec3(b.x-cx/2, b.y+size.y/2+cy, b.z+cz/2);
    vec3_t r100 = vec3(b.x+cx/2, b.y+size.y/2, b.z-cz/2), r101 = vec3(b.x+cx/2, b.y+size.y/2, b.z+cz/2), r110 = vec3(b.x+cx/2, b.y+size.y/2+cy, b.z-cz/2), r111 = vec3(b.x+cx/2, b.y+size.y/2+cy, b.z+cz/2);
    uint32_t glass = 0x111820;
    mesh_add_triangle(m, r001, r011, r111, glass); mesh_add_triangle(m, r001, r111, r101, glass); mesh_add_triangle(m, r100, r110, r010, glass); mesh_add_triangle(m, r100, r010, r000, glass);
    mesh_add_triangle(m, r010, r110, r111, color); mesh_add_triangle(m, r010, r111, r011, color); mesh_add_triangle(m, r000, r010, r011, glass); mesh_add_triangle(m, r000, r011, r001, glass); mesh_add_triangle(m, r101, r111, r110, glass); mesh_add_triangle(m, r101, r110, r100, glass);
    mesh_finalize(m); return m;
}

static mesh_t* create_penguin_mesh(personality_t p) {
    mesh_t* m = mesh_create(); float s = p.size; vec3_t b = vec3(0,0,0), size = vec3(0.6f*s, 1.2f*s, 0.5f*s);
    vec3_t v000 = vec3(b.x-size.x/2, b.y-size.y/2, b.z-size.z/2), v001 = vec3(b.x-size.x/2, b.y-size.y/2, b.z+size.z/2), v010 = vec3(b.x-size.x/2, b.y+size.y/2, b.z-size.z/2), v011 = vec3(b.x-size.x/2, b.y+size.y/2, b.z+size.z/2);
    vec3_t v100 = vec3(b.x+size.x/2, b.y-size.y/2, b.z-size.z/2), v101 = vec3(b.x+size.x/2, b.y-size.y/2, b.z+size.z/2), v110 = vec3(b.x+size.x/2, b.y+size.y/2, b.z-size.z/2), v111 = vec3(b.x+size.x/2, b.y+size.y/2, b.z+size.z/2);
    mesh_add_triangle(m, v001, v011, v111, p.body_color); mesh_add_triangle(m, v001, v111, v101, p.body_color); mesh_add_triangle(m, v100, v110, v010, p.body_color); mesh_add_triangle(m, v100, v010, v000, p.body_color);
    mesh_add_triangle(m, v010, v110, v111, p.body_color); mesh_add_triangle(m, v010, v111, v011, p.body_color); mesh_add_triangle(m, v000, v001, v101, p.body_color); mesh_add_triangle(m, v000, v101, v100, p.body_color);
    mesh_add_triangle(m, v000, v010, v011, p.body_color); mesh_add_triangle(m, v000, v011, v001, p.body_color); mesh_add_triangle(m, v101, v111, v110, p.body_color); mesh_add_triangle(m, v101, v110, v100, p.body_color);
    float z_front = size.z/2 + 0.01f, bw = 0.4f*s, bh = 0.8f*s;
    vec3_t w1 = vec3(-bw/2, -bh/2, z_front), w2 = vec3( bw/2, -bh/2, z_front), w3 = vec3( bw/2,  bh/2, z_front), w4 = vec3(-bw/2,  bh/2, z_front);
    mesh_add_triangle(m, w1, w2, w3, p.belly_color); mesh_add_triangle(m, w1, w3, w4, p.belly_color);
    float by = size.y/2 + 0.05f;
    vec3_t bk1 = vec3(-0.1f*s, by, z_front), bk2 = vec3( 0.1f*s, by, z_front), bk3 = vec3( 0.0f,   by, z_front + 0.3f*s);
    mesh_add_triangle(m, bk1, bk2, bk3, 0xFFAA00); mesh_finalize(m); return m;
}

/* ── Sky & Rendering ─────────────────────────────────────────── */
static void fill_sky(framebuffer_t* fb, const config_t* cfg) {
    for (int y = 0; y < fb->height; ++y) {
        float t = (float)y / (float)(fb->height - 1);
        float r0 = (float)((cfg->color_sky_top    >> 16) & 0xFF), g0 = (float)((cfg->color_sky_top    >>  8) & 0xFF), b0 = (float)( cfg->color_sky_top           & 0xFF);
        float r1 = (float)((cfg->color_sky_bottom >> 16) & 0xFF), g1 = (float)((cfg->color_sky_bottom >>  8) & 0xFF), b1 = (float)( cfg->color_sky_bottom        & 0xFF);
        float r = r0 + (r1 - r0) * t, g = g0 + (g1 - g0) * t, b = b0 + (b1 - b0) * t;
        float d = t - 0.62f, glow = expf(-d * d * 60.0f) * 0.4f;
        r += ((cfg->color_horizon_glow >> 16) & 0xFF) * glow; g += ((cfg->color_horizon_glow >>  8) & 0xFF) * glow; b += ( cfg->color_horizon_glow        & 0xFF) * glow;
        if (r > 255.0f) r = 255.0f;
        if (g > 255.0f) g = 255.0f;
        if (b > 255.0f) b = 255.0f;
        uint32_t px = rgba_pack((uint8_t)r, (uint8_t)g, (uint8_t)b, 0xFF);
        uint32_t* row = &fb->pixels[(size_t)y * (size_t)fb->width];
        for (int x = 0; x < fb->width; ++x) row[x] = px;
    }
}

static void render_object(framebuffer_t* fb, const mat4_t* vp, const object_t* obj, const lighting_t* light, int fb_w, int fb_h) {
    if (!obj->mesh || !obj->visible) return;
    mat4_t model = mat4_identity();
    model = mat4_mul(model, mat4_translate(obj->position));
    model = mat4_mul(model, mat4_rotate_z(obj->rotation.z));
    model = mat4_mul(model, mat4_rotate_y(obj->rotation.y));
    model = mat4_mul(model, mat4_rotate_x(obj->rotation.x));
    model = mat4_mul(model, mat4_scale(obj->scale));
    mat4_t mvp = mat4_mul(*vp, model);
    for (size_t i = 0; i < obj->mesh->count; ++i) {
        const triangle_t* tri = &obj->mesh->tris[i];
        vec4_t wa = mat4_mul_vec4(model, vec4_from_vec3(tri->a, 1.0f)), wb = mat4_mul_vec4(model, vec4_from_vec3(tri->b, 1.0f)), wc = mat4_mul_vec4(model, vec4_from_vec3(tri->c, 1.0f));
        vec3_t A = vec3(wa.x, wa.y, wa.z), B = vec3(wb.x, wb.y, wb.z), C = vec3(wc.x, wc.y, wc.z);
        vec3_t n = vec3_normalize(vec3_cross(vec3_sub(B, A), vec3_sub(C, A)));
        vec4_t ca = mat4_mul_vec4(mvp, vec4_from_vec3(tri->a, 1.0f)), cb = mat4_mul_vec4(mvp, vec4_from_vec3(tri->b, 1.0f)), cc = mat4_mul_vec4(mvp, vec4_from_vec3(tri->c, 1.0f));
        if (ca.w <= 1e-4f || cb.w <= 1e-4f || cc.w <= 1e-4f) continue;
        vec3_t na = vec4_perspective_divide(ca), nb = vec4_perspective_divide(cb), nc = vec4_perspective_divide(cc);
        screen_vertex_t sa = { (na.x*0.5f+0.5f)*(float)fb_w, (1.0f-(na.y*0.5f+0.5f))*(float)fb_h, (na.z*0.5f+0.5f) };
        screen_vertex_t sb = { (nb.x*0.5f+0.5f)*(float)fb_w, (1.0f-(nb.y*0.5f+0.5f))*(float)fb_h, (nb.z*0.5f+0.5f) };
        screen_vertex_t sc = { (nc.x*0.5f+0.5f)*(float)fb_w, (1.0f-(nc.y*0.5f+0.5f))*(float)fb_h, (nc.z*0.5f+0.5f) };
        rasterizer_triangle(fb, sa, sb, sc, lighting_shade(light, n, tri->color));
    }
}

/* ── Main ────────────────────────────────────────────────────── */
int main(void) {
    if (!terminal_init()) return 1;
    config_t cfg; if (!config_load(&cfg, "config/default.conf")) config_defaults(&cfg);

    int base_w = 0, base_h = 0; terminal_framebuffer_size(&base_w, &base_h);
    const int SSAA = 2; framebuffer_t* fb = framebuffer_create(base_w * SSAA, base_h * SSAA);
    if (!fb) { terminal_shutdown(); return 1; }

    scene_t scene; scene_init(&scene);
    mesh_t* road = road_mesh_create(&cfg); scene_add(&scene, object_make(road, vec3_zero(), vec3_zero(), vec3(1,1,1)));
    size_t light_idx = scene.count; mesh_t* light_mesh = light_mesh_create(LIGHT_GREEN, LIGHT_RED, &cfg); scene_add(&scene, object_make(light_mesh, vec3(10.0f, 0, 8.0f), vec3_zero(), vec3(1,1,1)));

    const int NUM_CARS = 30; car_agent_t cars[NUM_CARS];
    for (int i = 0; i < NUM_CARS; i++) {
        int lane = (i % 2 == 0) ? -1 : 1;
        float start_z = (lane == 1) ? -80.0f + (float)(i / 2) * 10.0f : 80.0f - (float)(i / 2) * 10.0f;
        mesh_t* cm = create_car_mesh(0); size_t idx = scene.count; scene_add(&scene, object_make(cm, vec3_zero(), vec3_zero(), vec3(1,1,1)));
        car_agent_init(&cars[i], lane, start_z, idx, &cfg, i * 1337);
        mesh_destroy(cm); cm = create_car_mesh(cars[i].color); scene.objects[idx].mesh = cm; scene.objects[idx].position = cars[i].position;
    }

    const int NUM_PENGUINS = 15; penguin_agent_t penguins[NUM_PENGUINS];
    for (int i = 0; i < NUM_PENGUINS; i++) {
        int dir = (i % 2 == 0) ? 1 : -1;
        penguin_agent_init(&penguins[i], dir, 0, &cfg, i * 7331);
        mesh_t* pm = create_penguin_mesh(penguins[i].personality); size_t idx = scene.count;
        scene_add(&scene, object_make(pm, penguins[i].position, vec3_zero(), vec3(1,1,1))); penguins[i].object_index = idx;
    }

    traffic_sm_t sm;
    traffic_sm_init(&sm);

    camera_t cam = camera_make(vec3(25.0f, 20.0f, 45.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), 45.0f * (3.14159265f / 180.0f), 0.1f, 150.0f);
    lighting_t light = lighting_default(); light.ambient = 0.35f;

    const double target_dt = 1.0 / 30.0;
    double last = timing_now_seconds();
    bool running = true;
    light_state_t last_v_state = LIGHT_GREEN, last_p_state = LIGHT_RED;

    while (running) {
        unsigned key = terminal_poll_key();
        if (key == (unsigned)cfg.key_quit || key == 'q' || key == 27) running = false;

        double now = timing_now_seconds();
        double dt = now - last;
        if (dt > 0.25) dt = 0.25;
        last = now;

        if (key == (unsigned)cfg.key_pedestrian_call) {
            traffic_sm_press_button(&sm, &cfg, now);
        }

        traffic_sm_update(&sm, dt, &cfg);

        light_state_t v_state = traffic_sm_get_vehicle_light(&sm, &cfg);
        light_state_t p_state = traffic_sm_get_pedestrian_light(&sm, &cfg);
        if (v_state != last_v_state || p_state != last_p_state) {
            mesh_destroy(light_mesh); light_mesh = light_mesh_create(v_state, p_state, &cfg);
            scene.objects[light_idx].mesh = light_mesh; last_v_state = v_state; last_p_state = p_state;
        }

        for (int i = 0; i < NUM_CARS; i++) {
            car_agent_update(&cars[i], cars, NUM_CARS, dt, v_state, &cfg);
            scene.objects[cars[i].object_index].position = cars[i].position;
        }

        bool is_walk_phase = (sm.current_phase == PHASE_PED_WALK);
        for (int i = 0; i < NUM_PENGUINS; i++) {
            penguin_agent_update(&penguins[i], dt, is_walk_phase);
            scene.objects[penguins[i].object_index].position = penguins[i].position;
            float waddle = sinf(penguins[i].waddle_phase) * penguins[i].personality.waddle_amplitude;
            scene.objects[penguins[i].object_index].rotation.y = waddle;
            if (fabsf(penguins[i].position.x) > 25.0f) penguin_agent_init(&penguins[i], (i % 2 == 0) ? 1 : -1, penguins[i].object_index, &cfg, (uint32_t)(now * 1000.0f) + i);
        }

        fill_sky(fb, &cfg);
        framebuffer_clear_depth(fb);

        /* --- Render 3D Scene FIRST --- */
        float aspect = (float)(base_w * SSAA) / (float)(base_h * SSAA);
        mat4_t vp = camera_view_projection(&cam, aspect);
        for (size_t i = 0; i < scene.count; ++i) render_object(fb, &vp, &scene.objects[i], &light, base_w * SSAA, base_h * SSAA);

        /* --- Draw Text HUD OVER THE 3D SCENE --- */
        uint32_t bright_green = rgba_pack(0, 255, 0, 255);
        uint32_t bright_yellow = rgba_pack(255, 255, 0, 255);
        uint32_t bright_red = rgba_pack(255, 0, 0, 255);

        uint32_t v_color = bright_red;
        uint32_t p_color = bright_red;
        const char* v_text = "VEHICLE RED";
        const char* p_text = "PEDESTRIAN RED";

        if (sm.current_phase == PHASE_STEADY_GREEN) { v_text = "VEHICLE GREEN"; v_color = bright_green; }
        else if (sm.current_phase == PHASE_VEH_GREEN_TO_YELLOW) { v_text = "VEHICLE FLASHING YELLOW"; v_color = bright_yellow; }
        else if (sm.current_phase == PHASE_VEH_YELLOW_STEADY) { v_text = "VEHICLE YELLOW"; v_color = bright_yellow; }
        else if (sm.current_phase == PHASE_VEH_RED_TO_GREEN) { v_text = "VEHICLE FLASHING GREEN"; v_color = bright_green; }
        else if (sm.current_phase == PHASE_PED_WALK) { p_text = "PEDESTRIAN GREEN"; p_color = bright_green; }
        else if (sm.current_phase == PHASE_PED_BLINK) { p_text = "PEDESTRIAN FLASHING GREEN"; p_color = bright_green; }
        else if (sm.current_phase == PHASE_PED_RED_TO_GREEN) { p_text = "PEDESTRIAN FLASHING GREEN"; p_color = bright_green; }

        int scale = 2;
        int margin = 2 * SSAA;
        draw_text(fb, margin, margin, v_text, v_color, scale);
        draw_text(fb, margin, margin + 10 * scale, p_text, p_color, scale);

        if (sm.queued_call) draw_text(fb, margin, margin + 20 * scale, "CALL QUEUED...", bright_yellow, scale);

        /* --- Draw Giant HUD Traffic Light OVER THE 3D SCENE --- */
        int hud_w = 16 * SSAA;
        int hud_h = 32 * SSAA;
        int hud_x = fb->width - hud_w - (4 * SSAA);
        int hud_y = (fb->height - hud_h) / 2;

        draw_rect(fb, hud_x - (2*SSAA), hud_y - (2*SSAA), hud_w + (4*SSAA), hud_h + (4*SSAA), 0x222222);

        int box_w = hud_w - (2*SSAA);
        int box_h = (hud_h / 2) - (2*SSAA);
        int top_y = hud_y + (1*SSAA);
        draw_rect(fb, hud_x + (1*SSAA), top_y, box_w, box_h, 0x111111);

        int bot_y = hud_y + (hud_h / 2) + (1*SSAA);
        draw_rect(fb, hud_x + (1*SSAA), bot_y, box_w, box_h, 0x111111);

        int icon_scale = SSAA;
        int icon_w = 12 * icon_scale;
        int icon_h = 12 * icon_scale;
        int icon_x = hud_x + (hud_w - icon_w) / 2;

        uint32_t current_v_color = (v_state == LIGHT_GREEN) ? bright_green : (v_state == LIGHT_YELLOW ? bright_yellow : bright_red);
        draw_icon(fb, icon_x, top_y + (box_h - icon_h)/2, ICON_CAR, current_v_color, icon_scale);

        uint32_t current_p_color = (p_state == LIGHT_GREEN) ? bright_green : bright_red;
        draw_icon(fb, icon_x, bot_y + (box_h - icon_h)/2, ICON_PENGUIN, current_p_color, icon_scale);

        terminal_present(fb);
        double spent = timing_now_seconds() - now;
        if (spent < target_dt) timing_sleep_seconds(target_dt - spent);
    }

    mesh_destroy(road); mesh_destroy(light_mesh);
    for (int i = 0; i < NUM_CARS; i++) mesh_destroy(scene.objects[cars[i].object_index].mesh);
    for (int i = 0; i < NUM_PENGUINS; i++) mesh_destroy(scene.objects[penguins[i].object_index].mesh);
    framebuffer_destroy(fb); terminal_shutdown(); return 0;
}