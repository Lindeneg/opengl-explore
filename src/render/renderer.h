#ifndef TRADING_STUFF_RENDERER_H_
#define TRADING_STUFF_RENDERER_H_

#include "../common.h"
#include "../core/mem.h"
#include "../math/math.h"
#include "mesh.h"

typedef struct {
    u32 program;
    i32 u_mvp;
    i32 u_tex;
    mat4_t view_proj;
} renderer_t;

void renderer_init(renderer_t *r, arena_t *scratch);
void renderer_begin(renderer_t *r, i32 fb_w, i32 fb_h, const mat4_t *view_proj, u32 texture);
void renderer_draw(renderer_t *r, const mesh_t *mesh, const mat4_t *model);
void renderer_shutdown(renderer_t *r);

#endif // TRADING_STUFF_RENDERER_H_
