#ifndef TRADING_STUFF_DEFS_H_
#define TRADING_STUFF_DEFS_H_

#include "../common.h"
#include "../core/mem.h"

#define DEF_ID_LEN 32
#define DEFS_MAX_RESOURCES 64

typedef enum {
    RES_AUTO,         // passengers, mail — auto-restocked in cities (no site)
    RES_RAW,          // extracted from resource sites
    RES_MANUFACTURED, // produced by industries
} resource_type_t;

typedef enum {
    RARITY_COMMON,
    RARITY_UNCOMMON,
    RARITY_RARE,
} rarity_t;

typedef struct {
    char id[DEF_ID_LEN];
    resource_type_t type;
    rarity_t rarity;
    f32 weight;                 // affects transport capacity
    f32 value_min, value_max;   // bounds for dynamic price generation
    f32 volatility;             // how much the price swings
    f32 site_capacity;          // default extractable amount at a RAW site (0 for non-raw)
    f32 site_replenish;         // default per-tick replenish at a RAW site (stubbed)
} resource_def_t;

// The reusable "rulebook" loaded from .def files. Grows later: industries, city tiers, skins.
typedef struct {
    resource_def_t resources[DEFS_MAX_RESOURCES];
    u32 resource_count;
} defs_t;

// Appends the file's definitions into `defs`; warns and skips malformed/unknown lines.
void defs_load_file(defs_t *defs, arena_t *scratch, const char *path);
const resource_def_t *defs_find_resource(const defs_t *defs, const char *id);

#endif // TRADING_STUFF_DEFS_H_
