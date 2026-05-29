#ifndef TRADING_STUFF_PATH_H_
#define TRADING_STUFF_PATH_H_

#include "mem.h"

// Join a NULL-terminated list of segments with the platform separator into `a`.
char *path_join(arena_t *a, ...);

// Wrapper so callers don't pass the sentinel by hand: FPATH(a, "x", "y", "z").
#define FPATH(a, ...) path_join((a), __VA_ARGS__, (const char *)NULL)

#endif // TRADING_STUFF_PATH_H_
