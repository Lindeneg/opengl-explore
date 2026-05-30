#ifndef TRADING_STUFF_WORLD_H_
#define TRADING_STUFF_WORLD_H_

#include "../common.h"
#include "../core/mem.h"
#include "../math/math.h"
#include "../render/assets.h"
#include "../render/renderer.h"
#include "level.h"

typedef enum {
    TERRAIN_GROUND,
    TERRAIN_WATER,
} terrain_t;

// object on a cell (building, tree, resource), ASSET_INVALID mesh means no object
typedef struct {
    mesh_handle mesh;
    texture_handle tex;  // atlas this object's mesh samples
    object_kind_t kind;  // whether it replaces the ground tile under it
    f32 scale;           // uniform scale (1 = native)
    u8 rot;              // quarter turns, 0..3
} tile_t;

// per-cell path bits, low 4 = neighbour connectivity (N=-z, E=+x, S=+z, W=-x),
// PRESENT marks a cell that holds a path at all (present-without-connections isn't empty)
enum {
    PATH_N = 1 << 0,
    PATH_E = 1 << 1,
    PATH_S = 1 << 2,
    PATH_W = 1 << 3,
    PATH_PRESENT = 1 << 4,
};

// city node on the grid, a trade endpoint connected by rail later
typedef struct {
    i32 cx, cz;
    u32 radius;
} city_t;

// RAW resource site, a cell that yields a resource (stock not yet simulated)
typedef struct {
    i32 cx, cz;
    u16 resource; // index into world.defs->resources
    f32 capacity, replenish, stock;
} site_t;

typedef struct {
    i32 w, h;
    f32 tile_size;
    u8 *terrain;   // w * h, terrain_t per cell
    tile_t *tiles; // w * h, objects on terrain (ASSET_INVALID = none)
    u8 *paths;     // w * h, PATH_* bitmask, 0 = no path
    city_t *cities;
    u32 city_count;
    site_t *sites;
    u32 site_count;
    const defs_t *defs; // resource rulebook this world was built from

    mesh_handle ground;      // terrain ground mesh, from the fill prototype
    texture_handle ground_tex;
    mesh_handle road_straight, road_corner, road_tsplit, road_junction;
    texture_handle road_tex; // atlas shared by road meshes
} world_t;

// compiles a level into a runtime world: loads assets, fills the grid, applies overrides, builds sites
world_t world_from_level(const level_t *level, const defs_t *defs, assets_t *assets,
                         arena_t *permanent, arena_t *scratch);
vec3_t world_cell_world(const world_t *world, i32 x, i32 z);
b32 world_world_to_cell(const world_t *world, vec3_t p, i32 *cx, i32 *cz);
void world_draw(const world_t *world, assets_t *assets, renderer_t *r);
void world_draw_paths(const world_t *world, assets_t *assets, renderer_t *r);

// path editing, set/clear update a cell and recompute connectivity for it + 4 neighbours
void world_path_set(world_t *world, i32 x, i32 z);
void world_path_clear(world_t *world, i32 x, i32 z);
b32 world_has_path(const world_t *world, i32 x, i32 z);
b32 world_cell_buildable(const world_t *world, i32 x, i32 z); // in-bounds + plain ground
// maps a connectivity mask (low 4 bits) to road mesh + quarter-turn rotation
void world_path_mesh(const world_t *world, u8 conn, mesh_handle *mesh, u8 *rot);

#endif // TRADING_STUFF_WORLD_H_
