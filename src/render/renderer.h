#ifndef TRADING_STUFF_RENDERER_H_
#define TRADING_STUFF_RENDERER_H_

#include "../common.h"
#include "../core/mem.h"
#include "../math/math.h"
#include "mesh.h"

typedef struct {
    u32 program; // textured mesh program
    i32 u_mvp;
    i32 u_tex;

    u32 color_program; // flat-colour program for untextured fills (water, overlays)
    i32 c_mvp;
    i32 c_color;
    mesh_t quad; // unit quad on XZ plane for renderer_fill_quad

    mat4_t view_proj;
    u32 bound_texture;  // last GL texture bound, skips redundant binds
    u32 active_program; // last program used, skips redundant glUseProgram
} renderer_t;

void renderer_init(renderer_t *r, arena_t *scratch);
void renderer_begin(renderer_t *r, i32 fb_w, i32 fb_h, const mat4_t *view_proj);
// each draw carries its texture (GL id), bound only when it changes
void renderer_draw(renderer_t *r, const mesh_t *mesh, const mat4_t *model, u32 texture);
// solid-colour unit quad (model places/scales it), for water and debug overlays
void renderer_fill_quad(renderer_t *r, const mat4_t *model, vec4_t color);
void renderer_shutdown(renderer_t *r);

#endif // TRADING_STUFF_RENDERER_H_
