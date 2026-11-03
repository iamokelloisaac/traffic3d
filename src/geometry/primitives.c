/*
 * primitives.c — build cube and plane triangle soup.
 *
 * Commit: feat(geometry): implement primitives_cube and primitives_plane
 *
 * Depends on: primitives.h
 */
#include "traffic3d/geometry/primitives.h"

mesh_t* primitives_cube(uint32_t rgb24_top, uint32_t rgb24_bottom,
                        uint32_t rgb24_side) {
    mesh_t* m = mesh_create();
    if (!m) return NULL;

    const float h = 0.5f;
    /* 8 corners. */
    vec3_t v000 = vec3(-h, -h, -h);
    vec3_t v001 = vec3(-h, -h,  h);
    vec3_t v010 = vec3(-h,  h, -h);
    vec3_t v011 = vec3(-h,  h,  h);
    vec3_t v100 = vec3( h, -h, -h);
    vec3_t v101 = vec3( h, -h,  h);
    vec3_t v110 = vec3( h,  h, -h);
    vec3_t v111 = vec3( h,  h,  h);

    /* Top face (+Y) — CCW seen from above. */
    mesh_add_triangle(m, v010, v110, v111, rgb24_top);
    mesh_add_triangle(m, v010, v111, v011, rgb24_top);

    /* Bottom face (-Y). */
    mesh_add_triangle(m, v000, v001, v101, rgb24_bottom);
    mesh_add_triangle(m, v000, v101, v100, rgb24_bottom);

    /* Front face (+Z). */
    mesh_add_triangle(m, v001, v011, v111, rgb24_side);
    mesh_add_triangle(m, v001, v111, v101, rgb24_side);

    /* Back face (-Z). */
    mesh_add_triangle(m, v100, v110, v010, rgb24_side);
    mesh_add_triangle(m, v100, v010, v000, rgb24_side);

    /* Right face (+X). */
    mesh_add_triangle(m, v101, v111, v110, rgb24_side);
    mesh_add_triangle(m, v101, v110, v100, rgb24_side);

    /* Left face (-X). */
    mesh_add_triangle(m, v000, v010, v011, rgb24_side);
    mesh_add_triangle(m, v000, v011, v001, rgb24_side);

    mesh_finalize(m);
    return m;
}

mesh_t* primitives_plane(float size, uint32_t rgb24) {
    mesh_t* m = mesh_create();
    if (!m) return NULL;
    float h = size * 0.5f;
    vec3_t a = vec3(-h, 0.0f, -h);
    vec3_t b = vec3( h, 0.0f, -h);
    vec3_t c = vec3( h, 0.0f,  h);
    vec3_t d = vec3(-h, 0.0f,  h);
    /* CCW when viewed from above (+Y looking down). */
    mesh_add_triangle(m, a, b, c, rgb24);
    mesh_add_triangle(m, a, c, d, rgb24);
    mesh_finalize(m);
    return m;
}