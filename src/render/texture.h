#ifndef TRADING_STUFF_TEXTURE_H_
#define TRADING_STUFF_TEXTURE_H_

#include "../common.h"
#include "../core/mem.h"

// `flip` flips rows on load. KayKit packs disagree: the City kit authors UVs bottom-left (flip),
// the Forest/Resource kits top-left (no flip), so this is declared per-texture in the level.
u32 texture_load(arena_t *scratch, const char *path, b32 flip);

#endif // TRADING_STUFF_TEXTURE_H_
