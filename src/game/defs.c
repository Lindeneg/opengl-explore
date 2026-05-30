#include "defs.h"

#include <string.h>

#include "../core/log.h"
#include "../core/text.h"

static void str_copy(char *dst, u64 cap, const char *src) {
    if (cap == 0)
        return;
    u64 i = 0;
    for (; src[i] && i < cap - 1; ++i)
        dst[i] = src[i];
    dst[i] = '\0';
}

static b32 res_type_parse(const char *s, resource_type_t *out) {
    if (strcmp(s, "auto") == 0) {
        *out = RES_AUTO;
        return true;
    }
    if (strcmp(s, "raw") == 0) {
        *out = RES_RAW;
        return true;
    }
    if (strcmp(s, "manufactured") == 0) {
        *out = RES_MANUFACTURED;
        return true;
    }
    return false;
}

static b32 rarity_parse(const char *s, rarity_t *out) {
    if (strcmp(s, "common") == 0) {
        *out = RARITY_COMMON;
        return true;
    }
    if (strcmp(s, "uncommon") == 0) {
        *out = RARITY_UNCOMMON;
        return true;
    }
    if (strcmp(s, "rare") == 0) {
        *out = RARITY_RARE;
        return true;
    }
    return false;
}

void defs_parse_resource(defs_t *d, char **cur) {
    char *id = text_next_token(cur), *type = text_next_token(cur), *rar = text_next_token(cur);
    char *w = text_next_token(cur), *vmin = text_next_token(cur), *vmax = text_next_token(cur);
    char *vol = text_next_token(cur), *cap = text_next_token(cur), *rep = text_next_token(cur);

    resource_type_t rt;
    rarity_t rr;
    if (!(id && type && rar && w && vmin && vmax && vol) || !res_type_parse(type, &rt) ||
        !rarity_parse(rar, &rr)) {
        LOG_WARN("defs: malformed resource line");
        return;
    }
    ASSERT_RET_V_MSG(d->resource_count < DEFS_MAX_RESOURCES, "defs: too many resources");

    resource_def_t *r = &d->resources[d->resource_count++];
    str_copy(r->id, sizeof r->id, id);
    r->type = rt;
    r->rarity = rr;
    r->weight = (f32)atof(w);
    r->value_min = (f32)atof(vmin);
    r->value_max = (f32)atof(vmax);
    r->volatility = (f32)atof(vol);
    r->site_capacity = cap ? (f32)atof(cap) : 0.0f;
    r->site_replenish = rep ? (f32)atof(rep) : 0.0f;
}

const resource_def_t *defs_find_resource(const defs_t *defs, const char *id) {
    for (u32 i = 0; i < defs->resource_count; ++i)
        if (strcmp(defs->resources[i].id, id) == 0)
            return &defs->resources[i];
    return NULL;
}

void defs_parse_tier(defs_t *d, char **cur) {
    char *id = text_next_token(cur), *rad = text_next_token(cur);
    char *maxb = text_next_token(cur), *slots = text_next_token(cur);
    if (!(id && rad && maxb && slots)) {
        LOG_WARN("defs: malformed tier line");
        return;
    }
    ASSERT_RET_V_MSG(d->tier_count < DEFS_MAX_TIERS, "defs: too many tiers");

    city_tier_def_t *t = &d->tiers[d->tier_count++];
    str_copy(t->id, sizeof t->id, id);
    t->radius = (u32)atoi(rad);
    t->max_buildings = (u32)atoi(maxb);
    t->industry_slots = (u32)atoi(slots);
}

const city_tier_def_t *defs_find_tier(const defs_t *defs, const char *id) {
    for (u32 i = 0; i < defs->tier_count; ++i)
        if (strcmp(defs->tiers[i].id, id) == 0)
            return &defs->tiers[i];
    return NULL;
}
