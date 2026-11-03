//
// Created by anonymous on 11/3/26.
//
/*
 * light_mesh.c — Implementation of the traffic light geometry.
 *
 * Commit: feat(geometry): implement traffic light pole and colored symbols
 *
 * Depends on: light_mesh.h
 */
#include "traffic3d/geometry/light_mesh.h"

static void add_box(mesh_t* m, vec3_t center, vec3_t size, uint32_t color) {
    float hx = size.x * 0.5f;
    float hy = size.y * 0.5f;
    float hz = size.z * 0.5f;

    vec3_t v000 = vec3(center.x - hx, center.y - hy, center.z - hz);
    vec3_t v001 = vec3(center.x - hx, center.y - hy, center.z + hz);
    vec3_t v010 = vec3(center.x - hx, center.y + hy, center.z - hz);
    vec3_t v011 = vec3(center.x - hx, center.y + hy, center.z + hz);
    vec3_t v100 = vec3(center.x + hx, center.y - hy, center.z - hz);
    vec3_t v101 = vec3(center.x + hx, center.y - hy, center.z + hz);
    vec3_t v110 = vec3(center.x + hx, center.y + hy, center.z - hz);
    vec3_t v111 = vec3(center.x + hx, center.y + hy, center.z + hz);

    /* Front, Back, Top, Bottom, Left, Right */
    mesh_add_triangle(m, v001, v011, v111, color); mesh_add_triangle(m, v001, v111, v101, color);
    mesh_add_triangle(m, v100, v110, v010, color); mesh_add_triangle(m, v100, v010, v000, color);
    mesh_add_triangle(m, v010, v110, v111, color); mesh_add_triangle(m, v010, v111, v011, color);
    mesh_add_triangle(m, v000, v001, v101, color); mesh_add_triangle(m, v000, v101, v100, color);
    mesh_add_triangle(m, v000, v010, v011, color); mesh_add_triangle(m, v000, v011, v001, color);
    mesh_add_triangle(m, v101, v111, v110, color); mesh_add_triangle(m, v101, v110, v100, color);
}

mesh_t* light_mesh_create(bool is_pedestrian, light_state_t state, const config_t* cfg) {
    mesh_t* m = mesh_create();
    if (!m) return NULL;

    /* 1. The Pole */
    add_box(m, vec3(0, 2.0f, 0), vec3(0.2f, 4.0f, 0.2f), 0x333333); /* Dark grey */

    /* 2. The Housing */
    float housing_y = 3.5f;
    float housing_h = is_pedestrian ? 1.2f : 0.8f;
    add_box(m, vec3(0, housing_y, 0.2f), vec3(1.2f, housing_h, 0.4f), 0x111111); /* Black */

    /* 3. The Symbol Color */
    uint32_t symbol_color = 0x440000; /* Dim red by default */
    if (state == LIGHT_GREEN) symbol_color = cfg->color_vehicle_go;
    else if (state == LIGHT_YELLOW) symbol_color = cfg->color_vehicle_warn;
    else if (state == LIGHT_RED) symbol_color = cfg->color_vehicle_stop;

    /* 4. The Symbol Geometry (drawn slightly in front of the housing) */
    float z_front = 0.42f;
    if (is_pedestrian) {
        /* Tall thin boxes to look like a person */
        add_box(m, vec3(0, housing_y + 0.3f, z_front), vec3(0.3f, 0.3f, 0.1f), symbol_color); /* Head */
        add_box(m, vec3(0, housing_y - 0.2f, z_front), vec3(0.4f, 0.6f, 0.1f), symbol_color); /* Body */
    } else {
        /* Wide flat box to look like a car */
        add_box(m, vec3(0, housing_y, z_front), vec3(0.8f, 0.4f, 0.1f), symbol_color);
    }

    mesh_finalize(m);
    return m;
}