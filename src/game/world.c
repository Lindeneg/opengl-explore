#include "world.h"

#include <string.h>

#include "../core/log.h"

// engine handles for a level's declared assets, parallel to level_t.assets
typedef struct {
    mesh_handle mesh[LEVEL_MAX_ASSETS];
    texture_handle tex[LEVEL_MAX_ASSETS];
} resolved_assets_t;

static i32 asset_index(const level_t *l, const char *name) {
    for (u32 i = 0; i < l->asset_count; ++i)
        if (strcmp(l->assets[i].name, name) == 0)
            return (i32)i;
    return -1;
}

static mesh_handle object_mesh(const level_t *l, const resolved_assets_t *ra, const char *id) {
    const object_def_t *o = level_find_object(l, id);
    if (!o)
        return ASSET_INVALID;
    i32 ai = asset_index(l, o->mesh);
    return ai >= 0 ? ra->mesh[ai] : ASSET_INVALID;
}

static texture_handle object_tex(const level_t *l, const resolved_assets_t *ra, const char *id) {
    const object_def_t *o = level_find_object(l, id);
    if (!o)
        return ASSET_INVALID;
    i32 ai = asset_index(l, o->tex);
    return ai >= 0 ? ra->tex[ai] : ASSET_INVALID;
}

static void str_copy(char *dst, u64 cap, const char *src) {
    if (cap == 0)
        return;
    u64 i = 0;
    for (; src[i] && i < cap - 1; ++i)
        dst[i] = src[i];
    dst[i] = '\0';
}

// stable per-cell hash so a city's generated layout is identical every launch
static u32 cell_hash(i32 x, i32 z) {
    u32 h = ((u32)x * 73856093u) ^ ((u32)z * 19349663u);
    h ^= h >> 13;
    h *= 0x5bd1e995u;
    h ^= h >> 15;
    return h;
}

// fills each city's footprint with buildings from the level's building prototypes.
// deterministic (no save/RNG yet); skips water, out-of-bounds and occupied cells so explicit
// overrides/sites always win, and reserves the centre cell for a future industry.
static void generate_city_buildings(world_t *w, const level_t *lvl, const resolved_assets_t *ra,
                                    const defs_t *defs) {
    const char *palette[LEVEL_MAX_OBJECTS];
    u32 pal_count = 0;
    for (u32 i = 0; i < lvl->object_count; ++i)
        if (lvl->objects[i].kind == OBJ_BUILDING)
            palette[pal_count++] = lvl->objects[i].id;
    if (pal_count == 0)
        return;

    for (u32 ci = 0; ci < w->city_count; ++ci) {
        const city_t *c = &w->cities[ci];
        if (c->tier >= defs->tier_count)
            continue;
        const city_tier_def_t *td = &defs->tiers[c->tier];
        i32 r = (i32)td->radius;
        u32 placed = 0;
        for (i32 dz = -r; dz <= r && placed < td->max_buildings; ++dz) {
            for (i32 dx = -r; dx <= r && placed < td->max_buildings; ++dx) {
                if (dx == 0 && dz == 0)
                    continue; // reserve centre
                i32 x = c->cx + dx, z = c->cz + dz;
                if (x < 0 || x >= w->w || z < 0 || z >= w->h)
                    continue;
                if (((x + z) & 1) != ((c->cx + c->cz) & 1))
                    continue; // checkerboard leaves gaps for streets/industry
                i32 idx = z * w->w + x;
                if (w->terrain[idx] == TERRAIN_WATER || w->tiles[idx].mesh != ASSET_INVALID)
                    continue;
                u32 h = cell_hash(x, z);
                const char *proto = palette[h % pal_count];
                mesh_handle mesh = object_mesh(lvl, ra, proto);
                texture_handle tex = object_tex(lvl, ra, proto);
                if (mesh == ASSET_INVALID || tex == ASSET_INVALID)
                    continue;
                const object_def_t *od = level_find_object(lvl, proto);
                w->tiles[idx] = (tile_t){.mesh = mesh,
                                         .tex = tex,
                                         .kind = OBJ_BUILDING,
                                         .scale = od ? od->scale : 1.0f,
                                         .rot = (u8)((h >> 8) & 3)};
                ++placed;
            }
        }
    }
}

world_t world_from_level(const level_t *lvl, const defs_t *defs, assets_t *assets,
                         arena_t *permanent, arena_t *scratch) {
    resolved_assets_t ra;
    for (u32 i = 0; i < lvl->asset_count; ++i) {
        ra.mesh[i] = ASSET_INVALID;
        ra.tex[i] = ASSET_INVALID;
        if (lvl->assets[i].kind == ASSET_MESH)
            ra.mesh[i] = assets_load_mesh(assets, scratch, lvl->assets[i].path);
        else
            ra.tex[i] =
                assets_load_texture(assets, scratch, lvl->assets[i].path, lvl->assets[i].flip);
    }

    world_t w = {0};
    w.w = lvl->w;
    w.h = lvl->h;
    w.tile_size = lvl->tile_size;
    w.ground = object_mesh(lvl, &ra, lvl->fill);
    w.ground_tex = object_tex(lvl, &ra, lvl->fill);
    w.road_straight = object_mesh(lvl, &ra, "road_straight");
    w.road_corner = object_mesh(lvl, &ra, "road_corner");
    w.road_tsplit = object_mesh(lvl, &ra, "road_tsplit");
    w.road_junction = object_mesh(lvl, &ra, "road_junction");
    w.road_tex = object_tex(lvl, &ra, "road_straight");

    u64 cells = (u64)w.w * (u64)w.h;
    w.terrain = push_array_z(permanent, u8, cells); // 0 means TERRAIN_GROUND
    w.paths = push_array_z(permanent, u8, cells);
    w.tiles = push_array(permanent, tile_t, cells);
    for (u64 i = 0; i < cells; ++i)
        w.tiles[i] = (tile_t){.mesh = ASSET_INVALID, .tex = ASSET_INVALID, .scale = 1.0f};

    for (u32 i = 0; i < lvl->water_count; ++i) {
        i32 x = lvl->water[i].x, z = lvl->water[i].z;
        if (x >= 0 && x < w.w && z >= 0 && z < w.h)
            w.terrain[z * w.w + x] = TERRAIN_WATER;
    }

    for (u32 i = 0; i < lvl->override_count; ++i) {
        const tile_override_t *t = &lvl->overrides[i];
        if (t->x < 0 || t->x >= w.w || t->z < 0 || t->z >= w.h)
            continue;
        const object_def_t *od = level_find_object(lvl, t->object);
        tile_t *cell = &w.tiles[t->z * w.w + t->x];
        cell->mesh = object_mesh(lvl, &ra, t->object);
        cell->tex = object_tex(lvl, &ra, t->object);
        cell->kind = od ? od->kind : OBJ_BUILDING;
        cell->scale = od ? od->scale : 1.0f;
        cell->rot = t->rot;
    }

    u32 ncities = lvl->city_count ? lvl->city_count : 1;
    w.cities = push_array(permanent, city_t, ncities);
    for (u32 i = 0; i < lvl->city_count; ++i) {
        const level_city_t *lc = &lvl->cities[i];
        const city_tier_def_t *td = defs_find_tier(defs, lc->tier);
        if (!td)
            LOG_WARN("level: city (%d,%d) references unknown tier '%s'", lc->cx, lc->cz, lc->tier);
        city_t *c = &w.cities[w.city_count++];
        c->cx = lc->cx;
        c->cz = lc->cz;
        c->tier = td ? (u16)(td - defs->tiers) : 0;
        c->radius = td ? td->radius : 0;
        str_copy(c->name, sizeof c->name, lc->name);
    }

    // per-game economic state: prices seeded mid-band, city supply/demand seeded with the
    // always-present (size-scaled) passengers + mail; everything else is filled by industries (2b)
    u32 nres = defs->resource_count;
    w.prices = push_array(permanent, f32, nres);
    for (u32 r = 0; r < nres; ++r)
        w.prices[r] = (defs->resources[r].value_min + defs->resources[r].value_max) * 0.5f;
    w.city_supply = push_array_z(permanent, f32, (u64)ncities * nres);
    w.city_demand = push_array_z(permanent, f32, (u64)ncities * nres);
    {
        const resource_def_t *rp = defs_find_resource(defs, "passengers");
        const resource_def_t *rm = defs_find_resource(defs, "mail");
        i32 pax = rp ? (i32)(rp - defs->resources) : -1;
        i32 mail = rm ? (i32)(rm - defs->resources) : -1;
        for (u32 i = 0; i < w.city_count; ++i) {
            const city_t *c = &w.cities[i];
            f32 pop = c->tier < defs->tier_count ? (f32)defs->tiers[c->tier].max_buildings : 1.0f;
            if (pax >= 0) {
                w.city_supply[i * nres + (u32)pax] = pop;
                w.city_demand[i * nres + (u32)pax] = pop;
            }
            if (mail >= 0) {
                w.city_supply[i * nres + (u32)mail] = pop * 0.6f;
                w.city_demand[i * nres + (u32)mail] = pop * 0.6f;
            }
        }
    }

    // resource sites, runtime entity plus a visual in the object layer (reuses world_draw)
    w.defs = defs;
    u32 nsites = lvl->site_count ? lvl->site_count : 1;
    w.sites = push_array(permanent, site_t, nsites);
    for (u32 i = 0; i < lvl->site_count; ++i) {
        const level_site_t *ls = &lvl->sites[i];
        const resource_def_t *rd = defs_find_resource(defs, ls->resource);
        if (!rd) {
            LOG_WARN("level: site (%d,%d) references unknown resource '%s'", ls->x, ls->z,
                     ls->resource);
            continue;
        }
        if (ls->x >= 0 && ls->x < w.w && ls->z >= 0 && ls->z < w.h) {
            const object_def_t *od = level_find_object(lvl, ls->object);
            tile_t *cell = &w.tiles[ls->z * w.w + ls->x];
            cell->mesh = object_mesh(lvl, &ra, ls->object);
            cell->tex = object_tex(lvl, &ra, ls->object);
            cell->kind = od ? od->kind : OBJ_RESOURCE;
            cell->scale = od ? od->scale : 1.0f;
            cell->rot = 0;
        }
        f32 cap = ls->capacity > 0.0f ? ls->capacity : rd->site_capacity;
        f32 rep = ls->replenish > 0.0f ? ls->replenish : rd->site_replenish;
        w.sites[w.site_count++] = (site_t){.cx = ls->x,
                                           .cz = ls->z,
                                           .resource = (u16)(rd - defs->resources),
                                           .capacity = cap,
                                           .replenish = rep,
                                           .stock = cap};
    }

    generate_city_buildings(&w, lvl, &ra, defs);

    LOG_INFO("world: %u resources, %u cities, %u sites", defs->resource_count, w.city_count,
             w.site_count);
    for (u32 i = 0; i < w.site_count; ++i) {
        const site_t *s = &w.sites[i];
        LOG_INFO("  site (%d,%d) %s cap=%.0f replenish=%.2f", s->cx, s->cz,
                 defs->resources[s->resource].id, (double)s->capacity, (double)s->replenish);
    }

    return w;
}

vec3_t world_cell_world(const world_t *w, i32 x, i32 z) {
    return vec3(((f32)x - (f32)w->w * 0.5f) * w->tile_size, 0.0f,
                ((f32)z - (f32)w->h * 0.5f) * w->tile_size);
}

b32 world_world_to_cell(const world_t *w, vec3_t p, i32 *cx, i32 *cz) {
    i32 x = (i32)floorf(p.x / w->tile_size + (f32)w->w * 0.5f + 0.5f);
    i32 z = (i32)floorf(p.z / w->tile_size + (f32)w->h * 0.5f + 0.5f);
    if (x < 0 || x >= w->w || z < 0 || z >= w->h)
        return false;
    *cx = x;
    *cz = z;
    return true;
}

static void draw_cell(const world_t *w, assets_t *assets, renderer_t *r, mesh_handle mesh,
                      texture_handle tex, f32 scale, u8 rot, i32 x, i32 z) {
    if (mesh == ASSET_INVALID || tex == ASSET_INVALID)
        return;
    vec3_t pos = world_cell_world(w, x, z);
    mat4_t model = mat4_mul(mat4_translate(pos),
                            mat4_mul(mat4_rotate(vec3(0.0f, 1.0f, 0.0f), (f32)rot * (PI * 0.5f)),
                                     mat4_scale(vec3(scale, scale, scale))));
    renderer_draw(r, assets_mesh(assets, mesh), &model, assets_texture_gl(assets, tex));
}

static void draw_terrain(const world_t *w, assets_t *assets, renderer_t *r, i32 x, i32 z) {
    if (w->terrain[z * w->w + x] == TERRAIN_WATER) {
        vec3_t pos = world_cell_world(w, x, z);
        mat4_t model =
            mat4_mul(mat4_translate(pos), mat4_scale(vec3(w->tile_size, 1.0f, w->tile_size)));
        renderer_fill_quad(r, &model, (vec4_t){0.16f, 0.38f, 0.60f, 1.0f});
    } else {
        draw_cell(w, assets, r, w->ground, w->ground_tex, 1.0f, 0, x, z);
    }
}

void world_draw(const world_t *w, assets_t *assets, renderer_t *r) {
    for (i32 z = 0; z < w->h; ++z) {
        for (i32 x = 0; x < w->w; ++x) {
            i32 i = z * w->w + x;
            if (w->paths[i] & PATH_PRESENT)
                continue; // road tile (world_draw_paths) replaces terrain + object here
            tile_t o = w->tiles[i];
            b8 replace = o.mesh != ASSET_INVALID && object_kind_replaces_ground(o.kind);
            if (!replace)
                draw_terrain(w, assets, r, x, z);
            if (o.mesh != ASSET_INVALID)
                draw_cell(w, assets, r, o.mesh, o.tex, o.scale, o.rot, x, z);
        }
    }
}

void world_draw_paths(const world_t *w, assets_t *assets, renderer_t *r) {
    for (i32 z = 0; z < w->h; ++z) {
        for (i32 x = 0; x < w->w; ++x) {
            u8 p = w->paths[z * w->w + x];
            if (!(p & PATH_PRESENT))
                continue;
            mesh_handle mesh;
            u8 rot;
            world_path_mesh(w, p, &mesh, &rot);
            draw_cell(w, assets, r, mesh, w->road_tex, 1.0f, rot, x, z);
        }
    }
}

b32 world_has_path(const world_t *w, i32 x, i32 z) {
    if (x < 0 || x >= w->w || z < 0 || z >= w->h)
        return false;
    return (w->paths[z * w->w + x] & PATH_PRESENT) != 0;
}

b32 world_cell_buildable(const world_t *w, i32 x, i32 z) {
    if (x < 0 || x >= w->w || z < 0 || z >= w->h)
        return false;
    i32 i = z * w->w + x;
    return w->terrain[i] == TERRAIN_GROUND && w->tiles[i].mesh == ASSET_INVALID;
}

static void recompute_mask(world_t *w, i32 x, i32 z) {
    i32 i = z * w->w + x;
    if (!(w->paths[i] & PATH_PRESENT)) {
        w->paths[i] = 0;
        return;
    }
    u8 m = PATH_PRESENT;
    if (world_has_path(w, x, z - 1))
        m |= PATH_N;
    if (world_has_path(w, x + 1, z))
        m |= PATH_E;
    if (world_has_path(w, x, z + 1))
        m |= PATH_S;
    if (world_has_path(w, x - 1, z))
        m |= PATH_W;
    w->paths[i] = m;
}

static void recompute_around(world_t *w, i32 x, i32 z) {
    recompute_mask(w, x, z);
    if (z > 0)
        recompute_mask(w, x, z - 1);
    if (x < w->w - 1)
        recompute_mask(w, x + 1, z);
    if (z < w->h - 1)
        recompute_mask(w, x, z + 1);
    if (x > 0)
        recompute_mask(w, x - 1, z);
}

void world_path_set(world_t *w, i32 x, i32 z) {
    ASSERT_RET_V(x >= 0 && x < w->w && z >= 0 && z < w->h);
    w->paths[z * w->w + x] |= PATH_PRESENT;
    recompute_around(w, x, z);
}

void world_path_clear(world_t *w, i32 x, i32 z) {
    ASSERT_RET_V(x >= 0 && x < w->w && z >= 0 && z < w->h);
    w->paths[z * w->w + x] &= (u8)~PATH_PRESENT;
    recompute_around(w, x, z);
}

// +90deg about +Y maps E->N, N->W, W->S, S->E
static u8 conn_rotate(u8 c) {
    u8 r = 0;
    if (c & PATH_E)
        r |= PATH_N;
    if (c & PATH_N)
        r |= PATH_W;
    if (c & PATH_W)
        r |= PATH_S;
    if (c & PATH_S)
        r |= PATH_E;
    return r;
}

static u8 conn_rot_to(u8 base, u8 target) {
    u8 c = base;
    for (u8 r = 0; r < 4; ++r) {
        if (c == target)
            return r;
        c = conn_rotate(c);
    }
    return 0;
}

static i32 conn_count(u8 c) { return (c & 1) + ((c >> 1) & 1) + ((c >> 2) & 1) + ((c >> 3) & 1); }

void world_path_mesh(const world_t *w, u8 conn, mesh_handle *mesh, u8 *rot) {
    // base orientations of the source meshes, flip these if a tile renders rotated wrong
    const u8 BASE_STRAIGHT = PATH_E | PATH_W;
    const u8 BASE_CORNER = PATH_N | PATH_E;
    const u8 BASE_TSPLIT = PATH_N | PATH_E | PATH_S;

    u8 c = conn & 0x0F;
    switch (conn_count(c)) {
    case 4:
        *mesh = w->road_junction;
        *rot = 0;
        return;
    case 3:
        *mesh = w->road_tsplit;
        *rot = conn_rot_to(BASE_TSPLIT, c);
        return;
    case 2:
        if (c == (PATH_N | PATH_S) || c == (PATH_E | PATH_W)) {
            *mesh = w->road_straight;
            *rot = conn_rot_to(BASE_STRAIGHT, c);
        } else {
            *mesh = w->road_corner;
            *rot = conn_rot_to(BASE_CORNER, c);
        }
        return;
    case 1:
        *mesh = w->road_straight;
        *rot = (c & (PATH_N | PATH_S)) ? conn_rot_to(BASE_STRAIGHT, PATH_N | PATH_S) : 0;
        return;
    default:
        *mesh = w->road_straight;
        *rot = 0;
        return;
    }
}
