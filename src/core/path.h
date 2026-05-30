#ifndef TRADING_STUFF_PATH_H_
#define TRADING_STUFF_PATH_H_

#include "mem.h"

char *path_join(arena_t *a, ...);

#define FPATH(a, ...) path_join((a), __VA_ARGS__, (const char *)NULL)

#endif // TRADING_STUFF_PATH_H_
