#ifndef TRADING_STUFF_BUILD_H_
#define TRADING_STUFF_BUILD_H_

#include "../common.h"
#include "../render/assets.h"
#include "../render/renderer.h"
#include "world.h"

typedef struct {
    b32 active; // drag in progress
    b32 erase;  // active drag is a bulldoze (right button)
    i32 sx, sz; // drag start cell
    i32 hx, hz; // last valid hovered cell
    b32 hover;  // hovered cell on the grid this frame
} build_tool_t;

// hover/hx/hz come from picking (main owns the camera), mouse buttons read internally
void build_tool_update(build_tool_t *t, world_t *w, b32 hover, i32 hx, i32 hz);
void build_tool_draw(const build_tool_t *t, const world_t *w, assets_t *assets, renderer_t *r);

#endif // TRADING_STUFF_BUILD_H_
