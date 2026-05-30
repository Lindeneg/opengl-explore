#ifndef TRADING_STUFF_SHADER_H_
#define TRADING_STUFF_SHADER_H_

#include "../common.h"
#include "../core/mem.h"

// compile + link a vertex/fragment program from source, returns GL program id (0 on failure, info
// log routed to the logger). scratch holds the temporary info-log buffer, left unchanged
u32 shader_from_src(arena_t *scratch, const char *vs_src, const char *fs_src);
u32 shader_from_files(arena_t *scratch, const char *vs_path, const char *fs_path);

#endif // TRADING_STUFF_SHADER_H_
