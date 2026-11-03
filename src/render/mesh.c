/*
* mesh.c — dynamic triangle array.
 *
 * Commit: feat(render): implement mesh_create/add/finalize/destroy
 *
 * Depends on: mesh.h, <stdlib.h>
 */
#include "traffic3d/render/mesh.h"
#include <stdlib.h>

mesh_t* mesh_create(void) {
    mesh_t* m = calloc(1, sizeof *m);
    if (!m) return NULL;
    m->capacity = 32;
    m->tris = malloc(sizeof(triangle_t) * m->capacity);
    if (!m->tris) { free(m); return NULL; }
    return m;
}

void mesh_destroy(mesh_t* m) {
    if (!m) return;
    free(m->tris);
    free(m);
}

void mesh_add_triangle(mesh_t* m, vec3_t a, vec3_t b, vec3_t c, uint32_t rgb24) {
    if (!m) return;
    if (m->count == m->capacity) {
        size_t cap = m->capacity * 2;
        triangle_t* grown = realloc(m->tris, sizeof(triangle_t) * cap);
        if (!grown) return; /* silently drop on OOM */
        m->tris = grown;
        m->capacity = cap;
    }
    m->tris[m->count++] = (triangle_t){
        .a = a, .b = b, .c = c,
        .normal = vec3_zero(),
        .color = rgb24,
    };
}

void mesh_finalize(mesh_t* m) {
    if (!m) return;
    for (size_t i = 0; i < m->count; ++i) {
        triangle_t* t = &m->tris[i];
        vec3_t ab = vec3_sub(t->b, t->a);
        vec3_t ac = vec3_sub(t->c, t->a);
        t->normal = vec3_normalize(vec3_cross(ab, ac));
    }
}