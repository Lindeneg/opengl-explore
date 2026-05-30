#include "assets.h"

#include <string.h>

#include "texture.h"

static const char *arena_strdup(arena_t *a, const char *s) {
    u64 n = strlen(s) + 1;
    char *p = push_array(a, char, n);
    memcpy(p, s, n);
    return p;
}

void assets_init(assets_t *a, arena_t *permanent) {
    *a = (assets_t){0};
    a->permanent = permanent;
}

mesh_handle assets_load_mesh(assets_t *a, arena_t *scratch, const char *path) {
    for (u32 i = 0; i < a->mesh_count; ++i)
        if (strcmp(a->mesh_paths[i], path) == 0)
            return i;

    ASSERT_MSG(a->mesh_count < ASSETS_MAX_MESHES, "assets: mesh capacity exceeded");
    u32 h = a->mesh_count++;
    a->meshes[h] = mesh_load_gltf(scratch, path);
    a->mesh_paths[h] = arena_strdup(a->permanent, path);
    return h;
}

texture_handle assets_load_texture(assets_t *a, arena_t *scratch, const char *path) {
    for (u32 i = 0; i < a->texture_count; ++i)
        if (strcmp(a->texture_paths[i], path) == 0)
            return i;

    ASSERT_MSG(a->texture_count < ASSETS_MAX_TEXTURES, "assets: texture capacity exceeded");
    u32 h = a->texture_count++;
    a->textures[h] = texture_load(scratch, path);
    a->texture_paths[h] = arena_strdup(a->permanent, path);
    return h;
}

mesh_t *assets_mesh(assets_t *a, mesh_handle h) { return &a->meshes[h]; }
u32 assets_texture_gl(assets_t *a, texture_handle h) { return a->textures[h]; }
