#include "build.h"

#include "../math/math.h"
#include "../platform/input.h"

#define BUILD_RUN_MAX 256

typedef struct {
    i32 x, z;
} cell_t;

// L-shaped run from (sx,sz) to (ex,ez): horizontal leg first, then vertical. Corner not doubled.
static i32 build_run(i32 sx, i32 sz, i32 ex, i32 ez, cell_t *out, i32 cap) {
    i32 n = 0;
    i32 stepx = (ex > sx) - (ex < sx);
    for (i32 x = sx;; x += stepx) {
        if (n < cap)
            out[n++] = (cell_t){x, sz};
        if (x == ex)
            break;
    }
    i32 stepz = (ez > sz) - (ez < sz);
    if (stepz != 0) {
        for (i32 z = sz + stepz;; z += stepz) {
            if (n < cap)
                out[n++] = (cell_t){ex, z};
            if (z == ez)
                break;
        }
    }
    return n;
}

static b8 in_run(const cell_t *run, i32 n, i32 x, i32 z) {
    for (i32 i = 0; i < n; ++i)
        if (run[i].x == x && run[i].z == z)
            return true;
    return false;
}

// A neighbour counts as connected if it already holds a path or is part of the pending run.
static b8 preview_present(const world_t *w, const cell_t *run, i32 n, i32 x, i32 z) {
    return world_has_path(w, x, z) || in_run(run, n, x, z);
}

void build_tool_update(build_tool_t *t, world_t *w, b32 hover, i32 hx, i32 hz) {
    t->hover = hover;
    if (hover) {
        t->hx = hx;
        t->hz = hz;
    }

    if (!t->active) {
        if (hover && input_mouse_pressed(MOUSE_LEFT)) {
            *t = (build_tool_t){.active = true,
                                .erase = false,
                                .sx = hx,
                                .sz = hz,
                                .hx = hx,
                                .hz = hz,
                                .hover = true};
        } else if (hover && input_mouse_pressed(MOUSE_RIGHT)) {
            *t = (build_tool_t){.active = true,
                                .erase = true,
                                .sx = hx,
                                .sz = hz,
                                .hx = hx,
                                .hz = hz,
                                .hover = true};
        }
        return;
    }

    b32 commit = t->erase ? input_mouse_released(MOUSE_RIGHT) : input_mouse_released(MOUSE_LEFT);
    if (!commit)
        return;

    cell_t run[BUILD_RUN_MAX];
    i32 n = build_run(t->sx, t->sz, t->hx, t->hz, run, BUILD_RUN_MAX);
    for (i32 i = 0; i < n; ++i) {
        if (t->erase)
            world_path_clear(w, run[i].x, run[i].z);
        else if (world_cell_buildable(w, run[i].x, run[i].z))
            world_path_set(w, run[i].x, run[i].z);
    }
    t->active = false;
}

void build_tool_draw(const build_tool_t *t, const world_t *w, assets_t *assets, renderer_t *r) {
    cell_t run[BUILD_RUN_MAX];
    i32 n;
    if (t->active && !t->erase)
        n = build_run(t->sx, t->sz, t->hx, t->hz, run, BUILD_RUN_MAX);
    else if (!t->active && t->hover)
        run[0] = (cell_t){t->hx, t->hz}, n = 1;
    else
        return; // bulldoze drag has no ghost (renderer has no tint to convey removal)

    for (i32 i = 0; i < n; ++i) {
        i32 x = run[i].x, z = run[i].z;
        if (!world_cell_buildable(w, x, z))
            continue;
        u8 conn = 0;
        if (preview_present(w, run, n, x, z - 1))
            conn |= PATH_N;
        if (preview_present(w, run, n, x + 1, z))
            conn |= PATH_E;
        if (preview_present(w, run, n, x, z + 1))
            conn |= PATH_S;
        if (preview_present(w, run, n, x - 1, z))
            conn |= PATH_W;

        mesh_handle mesh;
        u8 rot;
        world_path_mesh(w, conn, &mesh, &rot);
        vec3_t pos = world_cell_world(w, x, z);
        mat4_t model = mat4_mul(mat4_translate(pos),
                                mat4_rotate(vec3(0.0f, 1.0f, 0.0f), (f32)rot * (PI * 0.5f)));
        renderer_draw(r, assets_mesh(assets, mesh), &model);
    }
}
