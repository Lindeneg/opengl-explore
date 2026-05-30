#include "level.h"

#include <string.h>

#include "../core/file.h"
#include "../core/log.h"
#include "../core/text.h"

static FILE *os_fopen(const char *path, const char *mode) {
#if PLATFORM_WINDOWS
    FILE *f = NULL;
    fopen_s(&f, path, mode);
    return f;
#elif PLATFORM_LINUX
    return fopen(path, mode);
#endif
}

static void str_copy(char *dst, u64 cap, const char *src) {
    if (cap == 0)
        return;
    u64 i = 0;
    for (; src[i] && i < cap - 1; ++i)
        dst[i] = src[i];
    dst[i] = '\0';
}

static const char *obj_kind_str(object_kind_t k) {
    switch (k) {
    case OBJ_BUILDING:
        return "building";
    case OBJ_ROAD:
        return "road";
    case OBJ_INDUSTRY:
        return "industry";
    case OBJ_NATURE:
        return "nature";
    case OBJ_RESOURCE:
        return "resource";
    case OBJ_GROUND:
    default:
        return "ground";
    }
}

static b32 obj_kind_parse(const char *s, object_kind_t *out) {
    static const struct {
        const char *name;
        object_kind_t kind;
    } kinds[] = {
        {"ground", OBJ_GROUND},     {"building", OBJ_BUILDING}, {"road", OBJ_ROAD},
        {"industry", OBJ_INDUSTRY}, {"nature", OBJ_NATURE},     {"resource", OBJ_RESOURCE},
    };
    for (u64 i = 0; i < sizeof kinds / sizeof kinds[0]; ++i)
        if (strcmp(s, kinds[i].name) == 0) {
            *out = kinds[i].kind;
            return true;
        }
    return false;
}

const object_def_t *level_find_object(const level_t *l, const char *id) {
    for (u32 i = 0; i < l->object_count; ++i)
        if (strcmp(l->objects[i].id, id) == 0)
            return &l->objects[i];
    return NULL;
}

static void add_asset(level_t *l, const char *name, const char *path, asset_kind_t kind, b32 flip) {
    ASSERT_RET_V_MSG(l->asset_count < LEVEL_MAX_ASSETS, "level: too many assets");
    asset_decl_t *a = &l->assets[l->asset_count++];
    str_copy(a->name, sizeof a->name, name);
    str_copy(a->path, sizeof a->path, path);
    a->kind = kind;
    a->flip = flip;
}

static void add_object(level_t *l, const char *id, object_kind_t kind, const char *mesh,
                       const char *tex, f32 scale) {
    ASSERT_RET_V_MSG(l->object_count < LEVEL_MAX_OBJECTS, "level: too many objects");
    object_def_t *o = &l->objects[l->object_count++];
    str_copy(o->id, sizeof o->id, id);
    o->kind = kind;
    str_copy(o->mesh, sizeof o->mesh, mesh);
    str_copy(o->tex, sizeof o->tex, tex);
    o->scale = scale > 0.0f ? scale : 1.0f;
}

static void add_override(level_t *l, i32 x, i32 z, const char *id, u8 rot) {
    ASSERT_RET_V_MSG(l->override_count < LEVEL_MAX_OVERRIDES, "level: too many tile overrides");
    tile_override_t *t = &l->overrides[l->override_count++];
    t->x = x;
    t->z = z;
    str_copy(t->object, sizeof t->object, id);
    t->rot = rot;
}

static void add_city(level_t *l, i32 cx, i32 cz, const char *tier, const char *name) {
    ASSERT_RET_V_MSG(l->city_count < LEVEL_MAX_CITIES, "level: too many cities");
    level_city_t *c = &l->cities[l->city_count++];
    c->cx = cx;
    c->cz = cz;
    str_copy(c->tier, sizeof c->tier, tier);
    str_copy(c->name, sizeof c->name, name);
}

static void add_water(level_t *l, i32 x, i32 z) {
    ASSERT_RET_V_MSG(l->water_count < LEVEL_MAX_WATER, "level: too many water cells");
    l->water[l->water_count++] = (level_cell_t){x, z};
}

static void add_use(level_t *l, const char *path) {
    ASSERT_RET_V_MSG(l->use_count < LEVEL_MAX_USES, "level: too many use directives");
    str_copy(l->uses[l->use_count++], LEVEL_PATH_LEN, path);
}

static void add_site(level_t *l, i32 x, i32 z, const char *res, const char *obj, f32 cap, f32 rep) {
    ASSERT_RET_V_MSG(l->site_count < LEVEL_MAX_SITES, "level: too many sites");
    level_site_t *s = &l->sites[l->site_count++];
    s->x = x;
    s->z = z;
    str_copy(s->resource, sizeof s->resource, res);
    str_copy(s->object, sizeof s->object, obj);
    s->capacity = cap;
    s->replenish = rep;
}

static b32 parse_file(level_t *l, defs_t *defs, arena_t *scratch, const char *path);

static void use_file(level_t *l, defs_t *defs, arena_t *scratch, const char *path) {
    for (u32 i = 0; i < l->use_count; ++i)
        if (strcmp(l->uses[i], path) == 0)
            return;
    add_use(l, path);
    if (!parse_file(l, defs, scratch, path))
        LOG_WARN("level: use: cannot read %s", path);
}

static void parse_line(level_t *l, defs_t *defs, arena_t *scratch, char *line) {
    char *hash = strchr(line, '#');
    if (hash)
        *hash = '\0';

    char *cur = line;
    char *tok = text_next_token(&cur);
    if (!tok)
        return;

    if (strcmp(tok, "level") == 0) {
        char *rest = text_trim(cur); // rest of line, possibly quoted name
        if (*rest)
            str_copy(l->name, sizeof l->name, rest);
    } else if (strcmp(tok, "grid") == 0) {
        char *a = text_next_token(&cur), *b = text_next_token(&cur), *c = text_next_token(&cur);
        if (a)
            l->w = atoi(a);
        if (b)
            l->h = atoi(b);
        if (c)
            l->tile_size = (f32)atof(c);
    } else if (strcmp(tok, "use") == 0) {
        char *p = text_next_token(&cur);
        if (p)
            use_file(l, defs, scratch, p);
        else
            LOG_WARN("level: malformed use line");
    } else if (strcmp(tok, "resource") == 0) {
        defs_parse_resource(defs, &cur);
    } else if (strcmp(tok, "tier") == 0) {
        defs_parse_tier(defs, &cur);
    } else if (strcmp(tok, "texture") == 0 || strcmp(tok, "mesh") == 0) {
        asset_kind_t kind = tok[0] == 't' ? ASSET_TEXTURE : ASSET_MESH;
        char *name = text_next_token(&cur), *path = text_next_token(&cur),
             *fl = text_next_token(&cur);
        if (name && path)
            add_asset(l, name, path, kind, kind == ASSET_TEXTURE && fl && atoi(fl) != 0);
        else
            LOG_WARN("level: malformed asset line");
    } else if (strcmp(tok, "object") == 0) {
        char *id = text_next_token(&cur), *ks = text_next_token(&cur);
        char *mesh = text_next_token(&cur), *tex = text_next_token(&cur),
             *sc = text_next_token(&cur);
        object_kind_t kind;
        if (id && ks && mesh && tex && obj_kind_parse(ks, &kind))
            add_object(l, id, kind, mesh, tex, sc ? (f32)atof(sc) : 1.0f);
        else
            LOG_WARN("level: malformed object line");
    } else if (strcmp(tok, "fill") == 0) {
        char *id = text_next_token(&cur);
        if (id)
            str_copy(l->fill, sizeof l->fill, id);
    } else if (strcmp(tok, "city") == 0) {
        char *a = text_next_token(&cur), *b = text_next_token(&cur);
        char *tier = text_next_token(&cur), *name = text_next_token(&cur);
        if (a && b && tier)
            add_city(l, atoi(a), atoi(b), tier, name ? name : "");
        else
            LOG_WARN("level: malformed city line");
    } else if (strcmp(tok, "site") == 0) {
        char *a = text_next_token(&cur), *b = text_next_token(&cur);
        char *res = text_next_token(&cur), *obj = text_next_token(&cur);
        char *cap = text_next_token(&cur), *rep = text_next_token(&cur);
        if (a && b && res && obj)
            add_site(l, atoi(a), atoi(b), res, obj, cap ? (f32)atof(cap) : 0.0f,
                     rep ? (f32)atof(rep) : 0.0f);
        else
            LOG_WARN("level: malformed site line");
    } else if (strcmp(tok, "tile") == 0) {
        char *a = text_next_token(&cur), *b = text_next_token(&cur);
        char *id = text_next_token(&cur), *r = text_next_token(&cur);
        if (a && b && id)
            add_override(l, atoi(a), atoi(b), id, r ? (u8)atoi(r) : 0);
        else
            LOG_WARN("level: malformed tile line");
    } else if (strcmp(tok, "water") == 0) {
        char *a = text_next_token(&cur), *b = text_next_token(&cur);
        if (a && b)
            add_water(l, atoi(a), atoi(b));
        else
            LOG_WARN("level: malformed water line");
    } else if (strcmp(tok, "path") == 0) {
        // TODO: player paths persist here once building + UI lands
    } else {
        LOG_WARN("level: unknown directive '%s'", tok);
    }
}

static b32 parse_file(level_t *l, defs_t *defs, arena_t *scratch, const char *path) {
    file_data_t fd = file_read(scratch, path);
    if (!fd.data)
        return false;
    char *cur = (char *)fd.data;
    while (*cur) {
        char *eol = cur;
        while (*eol && *eol != '\n')
            ++eol;
        char saved = *eol;
        *eol = '\0';
        parse_line(l, defs, scratch, cur);
        if (saved == '\0')
            break;
        cur = eol + 1;
    }
    return true;
}

level_t *level_load(arena_t *scratch, defs_t *defs, const char *path) {
    level_t *l = push_struct_z(scratch, level_t);
    l->w = 32;
    l->h = 32;
    l->tile_size = 2.0f;
    str_copy(l->fill, sizeof l->fill, "ground");
    if (!parse_file(l, defs, scratch, path))
        return NULL;
    return l;
}

static const char *asset_kind_str(asset_kind_t k) {
    return k == ASSET_TEXTURE ? "texture" : "mesh";
}

b32 level_save(const level_t *l, const char *path) {
    FILE *f = os_fopen(path, "w");
    ASSERT_RET_MSG(f != NULL, false, "level_save: cannot open '%s'", path);

    fprintf(f, "# tradingstuff level\n");
    fprintf(f, "level \"%s\"\n", l->name);
    fprintf(f, "grid %d %d %g\n\n", l->w, l->h, (double)l->tile_size);

    for (u32 i = 0; i < l->use_count; ++i)
        fprintf(f, "use %s\n", l->uses[i]);
    if (l->use_count)
        fprintf(f, "\n");

    for (u32 i = 0; i < l->asset_count; ++i) {
        const asset_decl_t *a = &l->assets[i];
        if (a->kind == ASSET_TEXTURE)
            fprintf(f, "%s %s %s %d\n", asset_kind_str(a->kind), a->name, a->path, a->flip ? 1 : 0);
        else
            fprintf(f, "%s %s %s\n", asset_kind_str(a->kind), a->name, a->path);
    }
    fprintf(f, "\n");

    for (u32 i = 0; i < l->object_count; ++i) {
        const object_def_t *o = &l->objects[i];
        fprintf(f, "object %s %s %s %s %g\n", o->id, obj_kind_str(o->kind), o->mesh, o->tex,
                (double)o->scale);
    }
    fprintf(f, "\nfill %s\n\n", l->fill);

    for (u32 i = 0; i < l->city_count; ++i)
        fprintf(f, "city %d %d %s %s\n", l->cities[i].cx, l->cities[i].cz, l->cities[i].tier,
                l->cities[i].name);
    fprintf(f, "\n");

    for (u32 i = 0; i < l->water_count; ++i)
        fprintf(f, "water %d %d\n", l->water[i].x, l->water[i].z);
    fprintf(f, "\n");

    for (u32 i = 0; i < l->site_count; ++i) {
        const level_site_t *s = &l->sites[i];
        fprintf(f, "site %d %d %s %s %g %g\n", s->x, s->z, s->resource, s->object,
                (double)s->capacity, (double)s->replenish);
    }
    fprintf(f, "\n");

    for (u32 i = 0; i < l->override_count; ++i) {
        const tile_override_t *t = &l->overrides[i];
        fprintf(f, "tile %d %d %s %u\n", t->x, t->z, t->object, t->rot);
    }

    fclose(f);
    LOG_INFO("saved level to %s", path);
    return true;
}
