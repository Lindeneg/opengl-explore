#ifndef TRADING_STUFF_RENDERER_H_
#define TRADING_STUFF_RENDERER_H_

#include "../common.h"
#include "../core/mem.h"

typedef struct {
    u32 vao;
    u32 vbo;
    u32 program;
} renderer_t;

// Build the triangle's GL objects. `scratch` is used transiently for shader logs.
void renderer_init(renderer_t *r, arena_t *scratch);
void renderer_draw(renderer_t *r);
void renderer_shutdown(renderer_t *r);

#endif // TRADING_STUFF_RENDERER_H_
