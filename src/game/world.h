#ifndef TRADING_STUFF_WORLD_H_
#define TRADING_STUFF_WORLD_H_

#include "../common.h"
#include "../core/mem.h"
#include "../math/math.h"
#include "../render/assets.h"
#include "../render/renderer.h"
#include "level.h"

typedef struct {
    mesh_handle mesh; // ASSET_INVALID = empty
    u8 rot;           // quarter turns, 0..3
} tile_t;

// Per-cell path bits. Low 4 = neighbour connectivity (N=-z, E=+x, S=+z, W=-x),
// PRESENT marks a cell that holds a path at all (so "present, no connections" != "empty").
enum {
    PATH_N = 1 << 0,
    PATH_E = 1 << 1,
    PATH_S = 1 << 2,
    PATH_W = 1 << 3,
    PATH_PRESENT = 1 << 4,
};

// A city is a node on the grid: a trade endpoint that will grow and be connected by rail later.
typedef struct {
    i32 cx, cz;
    u32 radius;
} city_t;

typedef struct {
    i32 w, h;
    f32 tile_size;
    tile_t *tiles; // w * h
    u8 *paths;     // w * h, PATH_* bitmask; 0 = no path
    city_t *cities;
    u32 city_count;

    texture_handle atlas; // the texture renderer_begin binds for this world
    mesh_handle ground;
    mesh_handle road_straight, road_corner, road_tsplit, road_junction;
} world_t;

// Compiles a level into a runtime world: loads the level's declared assets into `assets`, fills the
// grid, applies tile overrides, and resolves the well-known road/ground prototypes.
world_t world_from_level(const level_t *level, assets_t *assets, arena_t *permanent,
                         arena_t *scratch);
vec3_t world_cell_world(const world_t *world, i32 x, i32 z);
b32 world_world_to_cell(const world_t *world, vec3_t p, i32 *cx, i32 *cz);
void world_draw(const world_t *world, assets_t *assets, renderer_t *r);
void world_draw_paths(const world_t *world, assets_t *assets, renderer_t *r);

// Path editing. set/clear update the cell and recompute connectivity for it and its 4 neighbours.
void world_path_set(world_t *world, i32 x, i32 z);
void world_path_clear(world_t *world, i32 x, i32 z);
b32 world_has_path(const world_t *world, i32 x, i32 z);
b32 world_cell_buildable(const world_t *world, i32 x, i32 z); // in-bounds + plain ground
// Maps a connectivity mask (low 4 bits) to the road mesh + quarter-turn rotation to draw.
void world_path_mesh(const world_t *world, u8 conn, mesh_handle *mesh, u8 *rot);

#endif // TRADING_STUFF_WORLD_H_
