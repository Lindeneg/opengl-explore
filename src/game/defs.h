#ifndef TRADING_STUFF_DEFS_H_
#define TRADING_STUFF_DEFS_H_

#include "../common.h"

#define DEF_ID_LEN 32
#define DEFS_MAX_RESOURCES 64
#define DEFS_MAX_TIERS 8

typedef enum {
    RES_AUTO,         // passengers, mail, auto-restocked in cities (no site)
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
    f32 weight;               // affects transport capacity
    f32 value_min, value_max; // bounds for dynamic price generation
    f32 volatility;           // how much the price swings
    f32 site_capacity;        // default extractable amount at a RAW site (0 for non-raw)
    f32 site_replenish;       // default per-tick replenish at a RAW site (stubbed)
} resource_def_t;

// city size tier: footprint + how much the generator fills it (rulebook, no per-game state)
typedef struct {
    char id[DEF_ID_LEN];
    u32 radius;         // footprint half-extent in cells -> a (2r+1)^2 square
    u32 max_buildings;  // generator budget within the footprint
    u32 industry_slots; // stored for later, unused this slice
} city_tier_def_t;

typedef struct {
    resource_def_t resources[DEFS_MAX_RESOURCES];
    u32 resource_count;
    city_tier_def_t tiers[DEFS_MAX_TIERS];
    u32 tier_count;
} defs_t;

void defs_parse_resource(defs_t *defs, char **cursor);
const resource_def_t *defs_find_resource(const defs_t *defs, const char *id);
void defs_parse_tier(defs_t *defs, char **cursor);
const city_tier_def_t *defs_find_tier(const defs_t *defs, const char *id);

#endif // TRADING_STUFF_DEFS_H_
