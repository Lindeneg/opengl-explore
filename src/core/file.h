#ifndef TRADING_STUFF_FILE_H_
#define TRADING_STUFF_FILE_H_

#include "../common.h"
#include "mem.h"

typedef struct {
    u8 *data;
    u64 size;
} file_data_t;

file_data_t file_read(arena_t *a, const char *path);

#endif // TRADING_STUFF_FILE_H_
