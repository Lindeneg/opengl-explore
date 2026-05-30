#ifndef TRADING_STUFF_LEVEL_H_
#define TRADING_STUFF_LEVEL_H_

#include "../common.h"
#include "../core/mem.h"
#include "defs.h"

#define LEVEL_ID_LEN 32
#define LEVEL_PATH_LEN 256
#define LEVEL_MAX_ASSETS 64
#define LEVEL_MAX_OBJECTS 64
#define LEVEL_MAX_OVERRIDES 8192
#define LEVEL_MAX_WATER 8192
#define LEVEL_MAX_CITIES 64
#define LEVEL_MAX_USES 16
#define LEVEL_MAX_SITES 256

typedef enum {
    ASSET_MESH,
    ASSET_TEXTURE,
} asset_kind_t;

// declared dependency: name the level refers to + the path to load it from
typedef struct {
    char name[LEVEL_ID_LEN];
    char path[LEVEL_PATH_LEN];
    asset_kind_t kind;
    b32 flip; // textures only: flip rows on load (KayKit packs disagree on UV origin)
} asset_decl_t;

typedef enum {
    OBJ_GROUND,   // terrain base mesh (used by fill)
    OBJ_BUILDING, // full tile with its own base, replaces the ground under it
    OBJ_ROAD,     // full tile, drawn via the path layer
    OBJ_INDUSTRY, // like a building, flagged as an industry site
    OBJ_NATURE,   // tree/bush, sits on the ground
    OBJ_RESOURCE, // resource site prop, sits on the ground
} object_kind_t;

// true for kinds whose mesh includes its own base (ground tile not drawn under them)
static inline b8 object_kind_replaces_ground(object_kind_t k) {
    return k == OBJ_BUILDING || k == OBJ_INDUSTRY;
}

// prototype, what a tile can be (references assets by name not path)
typedef struct {
    char id[LEVEL_ID_LEN];
    object_kind_t kind;
    char mesh[LEVEL_ID_LEN]; // asset_decl name
    char tex[LEVEL_ID_LEN];  // asset_decl name
    f32 scale;               // uniform scale when drawn (1 = native)
} object_def_t;

// non-fill cell, which prototype sits at (x,z) + its quarter-turn rotation
typedef struct {
    i32 x, z;
    char object[LEVEL_ID_LEN];
    u8 rot;
} tile_override_t;

typedef struct {
    i32 x, z;
} level_cell_t;

// city seed: position + which tier def to grow it from + display name. The generator
// (world_from_level) realizes buildings/industries into the world/save, not the level.
typedef struct {
    i32 cx, cz;
    char tier[LEVEL_ID_LEN];
    char name[LEVEL_ID_LEN];
} level_city_t;

// placed RAW resource site, references a resource def + the prototype that draws it
// capacity/replenish of 0 mean use the resource def default
typedef struct {
    i32 x, z;
    char resource[LEVEL_ID_LEN]; // resource def id
    char object[LEVEL_ID_LEN];   // visual prototype id
    f32 capacity, replenish;
} level_site_t;

// parsed source form of a world, world_from_level (world.h) compiles it into a runtime world_t
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

    char uses[LEVEL_MAX_USES][LEVEL_PATH_LEN]; // def files this level references
    u32 use_count;

    level_site_t sites[LEVEL_MAX_SITES];
    u32 site_count;
} level_t;

// parses a level file into scratch (NULL if missing), defs filled from its use directives
level_t *level_load(arena_t *scratch, defs_t *defs, const char *path);
b32 level_save(const level_t *level, const char *path);

const object_def_t *level_find_object(const level_t *level, const char *id);

#endif // TRADING_STUFF_LEVEL_H_
