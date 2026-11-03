/*
 * main.c — Entry point. Phase 5: Bringing the world to life.
 *
 * Commit: fix(main): spawn 30 cars in a stream and remove wheel flip glitch
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

/* ── Procedural Meshes for Agents ────────────────────────────── */

/* Creates a low-poly car mesh using boxes. Flatter and wider. */
static mesh_t* create_car_mesh(uint32_t color) {
    mesh_t* m = mesh_create();

    /* Main Body */
    vec3_t b = vec3(0,0,0);
    vec3_t size = vec3(1.8f, 0.6f, 3.8f);
    vec3_t v000 = vec3(b.x-size.x/2, b.y-size.y/2, b.z-size.z/2);
    vec3_t v001 = vec3(b.x-size.x/2, b.y-size.y/2, b.z+size.z/2);
    vec3_t v010 = vec3(b.x-size.x/2, b.y+size.y/2, b.z-size.z/2);
    vec3_t v011 = vec3(b.x-size.x/2, b.y+size.y/2, b.z+size.z/2);
    vec3_t v100 = vec3(b.x+size.x/2, b.y-size.y/2, b.z-size.z/2);
    vec3_t v101 = vec3(b.x+size.x/2, b.y-size.y/2, b.z+size.z/2);
    vec3_t v110 = vec3(b.x+size.x/2, b.y+size.y/2, b.z-size.z/2);
    vec3_t v111 = vec3(b.x+size.x/2, b.y+size.y/2, b.z+size.z/2);

    mesh_add_triangle(m, v001, v011, v111, color); mesh_add_triangle(m, v001, v111, v101, color);
    mesh_add_triangle(m, v100, v110, v010, color); mesh_add_triangle(m, v100, v010, v000, color);
    mesh_add_triangle(m, v010, v110, v111, color); mesh_add_triangle(m, v010, v111, v011, color);
    mesh_add_triangle(m, v000, v001, v101, color); mesh_add_triangle(m, v000, v101, v100, color);
    mesh_add_triangle(m, v000, v010, v011, color); mesh_add_triangle(m, v000, v011, v001, color);
    mesh_add_triangle(m, v101, v111, v110, color); mesh_add_triangle(m, v101, v110, v100, color);

    /* Roof / Cabin */
    float cx = 1.4f, cy = 0.5f, cz = 2.0f;
    vec3_t r000 = vec3(b.x-cx/2, b.y+size.y/2, b.z-cz/2);
    vec3_t r001 = vec3(b.x-cx/2, b.y+size.y/2, b.z+cz/2);
    vec3_t r010 = vec3(b.x-cx/2, b.y+size.y/2+cy, b.z-cz/2);
    vec3_t r011 = vec3(b.x-cx/2, b.y+size.y/2+cy, b.z+cz/2);
    vec3_t r100 = vec3(b.x+cx/2, b.y+size.y/2, b.z-cz/2);
    vec3_t r101 = vec3(b.x+cx/2, b.y+size.y/2, b.z+cz/2);
    vec3_t r110 = vec3(b.x+cx/2, b.y+size.y/2+cy, b.z-cz/2);
    vec3_t r111 = vec3(b.x+cx/2, b.y+size.y/2+cy, b.z+cz/2);

    uint32_t glass = 0x111820;
    /* Windshield & Windows */
    mesh_add_triangle(m, r001, r011, r111, glass); mesh_add_triangle(m, r001, r111, r101, glass);
    mesh_add_triangle(m, r100, r110, r010, glass); mesh_add_triangle(m, r100, r010, r000, glass);
    mesh_add_triangle(m, r010, r110, r111, color); mesh_add_triangle(m, r010, r111, r011, color);
    mesh_add_triangle(m, r000, r010, r011, glass); mesh_add_triangle(m, r000, r011, r001, glass);
    mesh_add_triangle(m, r101, r111, r110, glass); mesh_add_triangle(m, r101, r110, r100, glass);

    mesh_finalize(m);
    return m;
}

/* Creates a tiny penguin mesh (body, belly, beak) */
static mesh_t* create_penguin_mesh(personality_t p) {
    mesh_t* m = mesh_create();
    float s = p.size;
    vec3_t b = vec3(0,0,0);
    vec3_t size = vec3(0.6f*s, 1.2f*s, 0.5f*s);
    vec3_t v000 = vec3(b.x-size.x/2, b.y-size.y/2, b.z-size.z/2);
    vec3_t v001 = vec3(b.x-size.x/2, b.y-size.y/2, b.z+size.z/2);
    vec3_t v010 = vec3(b.x-size.x/2, b.y+size.y/2, b.z-size.z/2);
    vec3_t v011 = vec3(b.x-size.x/2, b.y+size.y/2, b.z+size.z/2);
    vec3_t v100 = vec3(b.x+size.x/2, b.y-size.y/2, b.z-size.z/2);
    vec3_t v101 = vec3(b.x+size.x/2, b.y-size.y/2, b.z+size.z/2);
    vec3_t v110 = vec3(b.x+size.x/2, b.y+size.y/2, b.z-size.z/2);
    vec3_t v111 = vec3(b.x+size.x/2, b.y+size.y/2, b.z+size.z/2);

    mesh_add_triangle(m, v001, v011, v111, p.body_color); mesh_add_triangle(m, v001, v111, v101, p.body_color);
    mesh_add_triangle(m, v100, v110, v010, p.body_color); mesh_add_triangle(m, v100, v010, v000, p.body_color);
    mesh_add_triangle(m, v010, v110, v111, p.body_color); mesh_add_triangle(m, v010, v111, v011, p.body_color);
    mesh_add_triangle(m, v000, v001, v101, p.body_color); mesh_add_triangle(m, v000, v101, v100, p.body_color);
    mesh_add_triangle(m, v000, v010, v011, p.body_color); mesh_add_triangle(m, v000, v011, v001, p.body_color);
    mesh_add_triangle(m, v101, v111, v110, p.body_color); mesh_add_triangle(m, v101, v110, v100, p.body_color);

    float z_front = size.z/2 + 0.01f;
    float bw = 0.4f*s, bh = 0.8f*s;
    vec3_t w1 = vec3(-bw/2, -bh/2, z_front);
    vec3_t w2 = vec3( bw/2, -bh/2, z_front);
    vec3_t w3 = vec3( bw/2,  bh/2, z_front);
    vec3_t w4 = vec3(-bw/2,  bh/2, z_front);
    mesh_add_triangle(m, w1, w2, w3, p.belly_color);
    mesh_add_triangle(m, w1, w3, w4, p.belly_color);

    float by = size.y/2 + 0.05f;
    vec3_t bk1 = vec3(-0.1f*s, by, z_front);
    vec3_t bk2 = vec3( 0.1f*s, by, z_front);
    vec3_t bk3 = vec3( 0.0f,   by, z_front + 0.3f*s);
    mesh_add_triangle(m, bk1, bk2, bk3, 0xFFAA00);

    mesh_finalize(m);
    return m;
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
        if (r > 255.0f) r = 255.0f; if (g > 255.0f) g = 255.0f; if (b > 255.0f) b = 255.0f;
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
    config_t cfg;
    if (!config_load(&cfg, "config/default.conf")) config_defaults(&cfg);

    int base_w = 0, base_h = 0;
    terminal_framebuffer_size(&base_w, &base_h);
    const int SSAA = 2;
    framebuffer_t* fb = framebuffer_create(base_w * SSAA, base_h * SSAA);
    if (!fb) { terminal_shutdown(); return 1; }

    scene_t scene;
    scene_init(&scene);

    mesh_t* road = road_mesh_create(&cfg);
    scene_add(&scene, object_make(road, vec3_zero(), vec3_zero(), vec3(1,1,1)));

    size_t light_idx = scene.count;
    mesh_t* light_mesh = light_mesh_create(LIGHT_GREEN, LIGHT_RED, &cfg);
    scene_add(&scene, object_make(light_mesh, vec3(10.0f, 0, 8.0f), vec3_zero(), vec3(1,1,1)));

    /*
     * SPAWN A STREAM OF CARS
     * We spawn 30 cars, spaced out along the Z axis.
     */
    const int NUM_CARS = 30;
    car_agent_t cars[NUM_CARS];

    for (int i = 0; i < NUM_CARS; i++) {
        int lane = (i % 2 == 0) ? -1 : 1;

        /* Space them out: cars in lane 1 start at -80 and go to +80.
           cars in lane -1 start at +80 and go to -80. */
        float start_z = 0.0f;
        if (lane == 1) {
            start_z = -80.0f + (float)(i / 2) * 10.0f;
        } else {
            start_z = 80.0f - (float)(i / 2) * 10.0f;
        }

        mesh_t* cm = create_car_mesh(0); /* temp */
        size_t idx = scene.count;
        scene_add(&scene, object_make(cm, vec3_zero(), vec3_zero(), vec3(1,1,1)));

        car_agent_init(&cars[i], lane, start_z, idx, &cfg, i * 1337);

        mesh_destroy(cm);
        cm = create_car_mesh(cars[i].color);
        scene.objects[idx].mesh = cm;
        scene.objects[idx].position = cars[i].position;
    }

    /* Spawn Penguins */
    const int NUM_PENGUINS = 15;
    penguin_agent_t penguins[NUM_PENGUINS];
    for (int i = 0; i < NUM_PENGUINS; i++) {
        int dir = (i % 2 == 0) ? 1 : -1;
        penguin_agent_init(&penguins[i], dir, 0, &cfg, i * 7331);

        mesh_t* pm = create_penguin_mesh(penguins[i].personality);
        size_t idx = scene.count;
        scene_add(&scene, object_make(pm, penguins[i].position, vec3_zero(), vec3(1,1,1)));
        penguins[i].object_index = idx;
    }

    traffic_sm_t sm;
    traffic_sm_init(&sm);

    camera_t cam = camera_make(vec3(25.0f, 20.0f, 45.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), 45.0f * (3.14159265f / 180.0f), 0.1f, 150.0f);
    lighting_t light = lighting_default();
    light.ambient = 0.35f;

    const double target_dt = 1.0 / 30.0;
    double last = timing_now_seconds();
    bool running = true;
    light_state_t last_v_state = LIGHT_GREEN, last_p_state = LIGHT_RED;

    while (running) {
        unsigned key = terminal_poll_key();
        if (key == (unsigned)cfg.key_quit || key == 'q' || key == 27) running = false;
        if (key == (unsigned)cfg.key_pedestrian_call) traffic_sm_press_button(&sm, &cfg);

        double now = timing_now_seconds();
        double dt = now - last; if (dt > 0.25) dt = 0.25; last = now;

        traffic_sm_update(&sm, dt, &cfg);

        light_state_t v_state = traffic_sm_get_vehicle_light(&sm, &cfg);
        light_state_t p_state = traffic_sm_get_pedestrian_light(&sm, &cfg);

        if (v_state != last_v_state || p_state != last_p_state) {
            mesh_destroy(light_mesh);
            light_mesh = light_mesh_create(v_state, p_state, &cfg);
            scene.objects[light_idx].mesh = light_mesh;
            last_v_state = v_state;
            last_p_state = p_state;
        }

        /* Update Cars (passing the whole array for queue logic) */
        for (int i = 0; i < NUM_CARS; i++) {
            car_agent_update(&cars[i], cars, NUM_CARS, dt, v_state, &cfg);
            scene.objects[cars[i].object_index].position = cars[i].position;
            /* REMOVED the rotation.x = wheel_rotation to stop the flipping! */
        }

        /* Update Penguins */
        for (int i = 0; i < NUM_PENGUINS; i++) {
            penguin_agent_update(&penguins[i], dt, p_state);
            scene.objects[penguins[i].object_index].position = penguins[i].position;

            float waddle = sinf(penguins[i].waddle_phase) * penguins[i].personality.waddle_amplitude;
            scene.objects[penguins[i].object_index].rotation.y = waddle;

            if (fabsf(penguins[i].position.x) > 25.0f) {
                penguin_agent_init(&penguins[i], (i % 2 == 0) ? 1 : -1, penguins[i].object_index, &cfg, (uint32_t)(now * 1000.0f) + i);
            }
        }

        fill_sky(fb, &cfg);
        framebuffer_clear_depth(fb);

        float aspect = (float)(base_w * SSAA) / (float)(base_h * SSAA);
        mat4_t vp = camera_view_projection(&cam, aspect);

        for (size_t i = 0; i < scene.count; ++i) {
            render_object(fb, &vp, &scene.objects[i], &light, base_w * SSAA, base_h * SSAA);
        }

        terminal_present(fb);

        double spent = timing_now_seconds() - now;
        if (spent < target_dt) timing_sleep_seconds(target_dt - spent);
    }

    mesh_destroy(road);
    mesh_destroy(light_mesh);
    for (int i = 0; i < NUM_CARS; i++) mesh_destroy(scene.objects[cars[i].object_index].mesh);
    for (int i = 0; i < NUM_PENGUINS; i++) mesh_destroy(scene.objects[penguins[i].object_index].mesh);
    framebuffer_destroy(fb);
    terminal_shutdown();
    return 0;
}