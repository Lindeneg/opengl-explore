#ifndef TRADING_STUFF_ASSETS_H_
#define TRADING_STUFF_ASSETS_H_

#include "../common.h"
#include "../core/mem.h"
#include "mesh.h"

typedef u32 mesh_handle;
typedef u32 texture_handle;
#define ASSET_INVALID ((u32)-1)

#define ASSETS_MAX_MESHES 256
#define ASSETS_MAX_TEXTURES 32

// Loads meshes/textures once, keyed by path; loading the same path twice returns the same handle.
typedef struct {
    mesh_t meshes[ASSETS_MAX_MESHES];
    const char *mesh_paths[ASSETS_MAX_MESHES];
    u32 mesh_count;

    u32 textures[ASSETS_MAX_TEXTURES]; // GL texture ids
    const char *texture_paths[ASSETS_MAX_TEXTURES];
    u32 texture_count;

    arena_t *permanent; // owns the cached path strings
} assets_t;

void assets_init(assets_t *a, arena_t *permanent);

mesh_handle assets_load_mesh(assets_t *a, arena_t *scratch, const char *path);
texture_handle assets_load_texture(assets_t *a, arena_t *scratch, const char *path, b32 flip);

mesh_t *assets_mesh(assets_t *a, mesh_handle h);
u32 assets_texture_gl(assets_t *a, texture_handle h);

#endif // TRADING_STUFF_ASSETS_H_
