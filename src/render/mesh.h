#ifndef TRADING_STUFF_MESH_H_
#define TRADING_STUFF_MESH_H_

#include "../common.h"
#include "../core/mem.h"

typedef struct {
    u32 vao;
    u32 vbo;
    u32 ebo;
    u32 index_count;
} mesh_t;

mesh_t mesh_load_gltf(arena_t *scratch, const char *path);
mesh_t mesh_make_quad(f32 size); // flat quad on the XZ plane, centred at origin (pos+uv layout)
void mesh_destroy(mesh_t *m);

#endif // TRADING_STUFF_MESH_H_
