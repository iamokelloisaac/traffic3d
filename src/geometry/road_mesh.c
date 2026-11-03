/*
 * road_mesh.c — Implementation of the road geometry.
 *
 * Commit: fix(geometry): adjust scene bounds to prevent camera clipping
 *
 * Depends on: road_mesh.h
 */
#include "traffic3d/geometry/road_mesh.h"

/* Helper to add a flat quad (2 triangles) at a specific y-height. */
static void add_quad(mesh_t* m, float x1, float z1, float x2, float z2, float y, uint32_t color) {
    vec3_t a = vec3(x1, y, z1);
    vec3_t b = vec3(x2, y, z1);
    vec3_t c = vec3(x2, y, z2);
    vec3_t d = vec3(x1, y, z2);
    mesh_add_triangle(m, a, b, c, color);
    mesh_add_triangle(m, a, c, d, color);
}

mesh_t* road_mesh_create(const config_t* cfg) {
    mesh_t* m = mesh_create();
    if (!m) return NULL;

    /*
     * Keep the world bounded. 80x80 total area.
     * The camera sits at Z=45, so nothing will clip behind it.
     */
    float ground_size = 80.0f;
    float road_width = 16.0f;
    float road_length = 60.0f;  /* -30 to +30 */
    float road_half_w = road_width / 2.0f;
    float road_half_l = road_length / 2.0f;

    /* 1. Grass / Ground */
    /* Drop it slightly to -0.2 so it never z-fights with the road */
    add_quad(m, -ground_size/2, -ground_size/2,
                ground_size/2,  ground_size/2,
                -0.2f, 0x1A2E1A);

    /* 2. Main Asphalt Road */
    add_quad(m, -road_half_w, -road_half_l,
                road_half_w,  road_half_l,
                0.0f, cfg->color_road);

    /* 3. Sidewalks */
    uint32_t sw_color = cfg->color_sidewalk;
    float sw_width = 4.0f;

    /* Left sidewalk */
    add_quad(m, -road_half_w - sw_width, -road_half_l,
              -road_half_w,               road_half_l, 0.05f, sw_color);
    /* Right sidewalk */
    add_quad(m,  road_half_w,               -road_half_l,
                road_half_w + sw_width,     road_half_l, 0.05f, sw_color);

    /* 4. Center Lane Dashed Markings */
    uint32_t line_color = cfg->color_road_marking;
    /* Raised to 0.1 to ensure it draws on top of the road */
    for (float z = -road_half_l; z < road_half_l; z += 8.0f) {
        add_quad(m, -0.2f, z, 0.2f, z + 4.0f, 0.1f, line_color);
    }

    /* 5. Zebra Crosswalk */
    /* Math checks out perfectly: 10 stripes * (1.0 width + 0.6 gap) = 16.0 wide */
    float stripe_width = 1.0f;
    float stripe_gap = 0.6f;
    float start_x = -road_half_w;
    float crosswalk_z = 10.0f;

    for (int i = 0; i < 10; i++) {
        float x1 = start_x + i * (stripe_width + stripe_gap);
        float x2 = x1 + stripe_width;
        add_quad(m, x1, crosswalk_z, x2, crosswalk_z + 4.0f, 0.15f, line_color);
    }

    mesh_finalize(m);
    return m;
}