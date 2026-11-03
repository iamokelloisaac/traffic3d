/*
 * light_mesh.c — Implementation of the combined traffic light.
 *
 * Commit: feat(geometry): implement single pole with car and pedestrian symbols
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

mesh_t* light_mesh_create(light_state_t vehicle_state,
                          light_state_t pedestrian_state,
                          const config_t* cfg) {
    mesh_t* m = mesh_create();
    if (!m) return NULL;

    /* 1. The Pole */
    add_box(m, vec3(0, 3.0f, 0), vec3(0.4f, 6.0f, 0.4f), 0x333333);

    /* 2. The Housing (Backing box) */
    add_box(m, vec3(0, 4.0f, 0.2f), vec3(2.2f, 3.6f, 0.4f), 0x111111);

    /* 3. Determine Colors */
    uint32_t v_color = cfg->color_vehicle_stop;
    if (vehicle_state == LIGHT_GREEN) v_color = cfg->color_vehicle_go;
    else if (vehicle_state == LIGHT_YELLOW) v_color = cfg->color_vehicle_warn;

    uint32_t p_color = cfg->color_pedestrian_stop;
    if (pedestrian_state == LIGHT_GREEN) p_color = cfg->color_pedestrian_go;

    float z_front = 0.45f; /* Drawn slightly in front of the housing */

    /* 4. Top Symbol: CAR (Horizontal) */
    float car_y = 5.0f;
    /* Main body */
    add_box(m, vec3(0, car_y, z_front), vec3(1.4f, 0.4f, 0.1f), v_color);
    /* Roof */
    add_box(m, vec3(0, car_y + 0.3f, z_front), vec3(0.8f, 0.3f, 0.1f), v_color);
    /* Wheels (dark cutouts to make it look like a car) */
    add_box(m, vec3(-0.4f, car_y - 0.2f, z_front + 0.01f), vec3(0.3f, 0.2f, 0.1f), 0x111111);
    add_box(m, vec3( 0.4f, car_y - 0.2f, z_front + 0.01f), vec3(0.3f, 0.2f, 0.1f), 0x111111);

    /* 5. Bottom Symbol: PEDESTRIAN (Vertical) */
    float ped_y = 3.0f;
    /* Head */
    add_box(m, vec3(0, ped_y + 0.6f, z_front), vec3(0.4f, 0.4f, 0.1f), p_color);
    /* Body */
    add_box(m, vec3(0, ped_y, z_front), vec3(0.5f, 0.7f, 0.1f), p_color);
    /* Legs */
    add_box(m, vec3(-0.2f, ped_y - 0.6f, z_front), vec3(0.2f, 0.4f, 0.1f), p_color);
    add_box(m, vec3( 0.2f, ped_y - 0.6f, z_front), vec3(0.2f, 0.4f, 0.1f), p_color);

    mesh_finalize(m);
    return m;
}