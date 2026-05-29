#ifndef TRADING_STUFF_SHADER_H_
#define TRADING_STUFF_SHADER_H_

#include "../common.h"
#include "../core/mem.h"

// Compile + link a vertex/fragment program from source. Returns the GL program
// id, or 0 on failure (info log routed to the logger). `scratch` holds the
// temporary info-log buffer and is left unchanged on return.
u32 shader_from_src(arena_t *scratch, const char *vs_src, const char *fs_src);

#endif // TRADING_STUFF_SHADER_H_
