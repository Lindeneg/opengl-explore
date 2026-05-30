#ifndef TRADING_STUFF_LEVEL_H_
#define TRADING_STUFF_LEVEL_H_

#include "../common.h"
#include "../core/mem.h"

#define LEVEL_ID_LEN 32
#define LEVEL_PATH_LEN 256
#define LEVEL_MAX_ASSETS 64
#define LEVEL_MAX_OBJECTS 64
#define LEVEL_MAX_OVERRIDES 8192
#define LEVEL_MAX_WATER 8192
#define LEVEL_MAX_CITIES 64

typedef enum {
    ASSET_MESH,
    ASSET_TEXTURE,
} asset_kind_t;

// A declared dependency: a name the level refers to + the path to load it from.
typedef struct {
    char name[LEVEL_ID_LEN];
    char path[LEVEL_PATH_LEN];
    asset_kind_t kind;
    b32 flip; // textures only: flip rows on load (KayKit packs disagree on UV origin)
} asset_decl_t;

typedef enum {
    OBJ_GROUND,   // terrain base mesh (used by `fill`)
    OBJ_BUILDING, // full tile incl. its own base -> replaces the ground under it
    OBJ_ROAD,     // full tile, drawn via the path layer
    OBJ_INDUSTRY, // like a building, but flagged as an industry site
    OBJ_NATURE,   // tree/bush -> sits on top of the ground
    OBJ_RESOURCE, // resource site prop -> sits on top of the ground
} object_kind_t;

// True for kinds whose mesh includes its own base (so the ground tile is not drawn under them).
static inline b8 object_kind_replaces_ground(object_kind_t k) {
    return k == OBJ_BUILDING || k == OBJ_INDUSTRY;
}

// A prototype: what a tile can be. References assets by name, not path.
typedef struct {
    char id[LEVEL_ID_LEN];
    object_kind_t kind;
    char mesh[LEVEL_ID_LEN]; // asset_decl name
    char tex[LEVEL_ID_LEN];  // asset_decl name
    f32 scale;               // uniform scale applied when drawn (1 = native)
} object_def_t;

// A non-fill cell: which prototype sits at (x,z) and its quarter-turn rotation.
typedef struct {
    i32 x, z;
    char object[LEVEL_ID_LEN];
    u8 rot;
} tile_override_t;

typedef struct {
    i32 x, z;
} level_cell_t;

typedef struct {
    i32 cx, cz;
    u32 radius;
} level_city_t;

// The serializable source form of a world: the generator and the file loader both produce this,
// and world_from_level (world.h) compiles it into a runtime world_t.
typedef struct {
    char name[LEVEL_ID_LEN];
    i32 w, h;
    f32 tile_size;

    asset_decl_t assets[LEVEL_MAX_ASSETS];
    u32 asset_count;

    object_def_t objects[LEVEL_MAX_OBJECTS];
    u32 object_count;

    char fill[LEVEL_ID_LEN]; // prototype id for every cell not covered by an override

    tile_override_t overrides[LEVEL_MAX_OVERRIDES];
    u32 override_count;

    level_cell_t water[LEVEL_MAX_WATER]; // cells whose terrain is water (else ground)
    u32 water_count;

    level_city_t cities[LEVEL_MAX_CITIES];
    u32 city_count;
} level_t;

// All allocate the returned level_t in `scratch`; load returns NULL if the file is missing.
level_t *level_generate(arena_t *scratch, u32 seed);
level_t *level_load(arena_t *scratch, const char *path);
b32 level_save(const level_t *level, const char *path);

const object_def_t *level_find_object(const level_t *level, const char *id);

#endif // TRADING_STUFF_LEVEL_H_
