#ifndef TRADING_STUFF_RENDERER_H_
#define TRADING_STUFF_RENDERER_H_

#include "../common.h"
#include "../core/mem.h"

typedef struct {
    u32 vao;
    u32 vbo;
    u32 ebo;
    u32 program;
    i32 u_mvp;
    u32 index_count;
} renderer_t;

void renderer_init(renderer_t *r, arena_t *scratch);
void renderer_draw(renderer_t *r, i32 fb_w, i32 fb_h, f64 t);
void renderer_shutdown(renderer_t *r);

#endif // TRADING_STUFF_RENDERER_H_
