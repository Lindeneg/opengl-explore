#ifndef TRADING_STUFF_MEM_H_
#define TRADING_STUFF_MEM_H_

#include "../common.h"

#define ARENA_DEFAULT_ALIGN 16
#define ARENA_COMMIT_CHUNK KiB(64)

typedef struct {
    u8 *base;
    u64 cap;
    u64 committed;
    u64 used;
} arena_t;

typedef struct {
    arena_t *arena;
    u64 used;
} temp_t;

void arena_sys_init(u64 reserve);
arena_t arena_create(u64 cap);

void *arena_push_aligned(arena_t *a, u64 size, u64 align);
void *arena_push(arena_t *a, u64 size);                 // ARENA_DEFAULT_ALIGN
void *arena_push_zero(arena_t *a, u64 size, u64 align); // zero-filled

void arena_reset(arena_t *a); // rewind used to 0, keep pages committed

temp_t temp_begin(arena_t *a);
void temp_end(temp_t t); // rewind to the temp_begin mark

#define push_struct(a, T) (T *)arena_push_aligned((a), sizeof(T), _Alignof(T))
#define push_array(a, T, n) (T *)arena_push_aligned((a), sizeof(T) * (u64)(n), _Alignof(T))
#define push_struct_z(a, T) (T *)arena_push_zero((a), sizeof(T), _Alignof(T))
#define push_array_z(a, T, n) (T *)arena_push_zero((a), sizeof(T) * (u64)(n), _Alignof(T))

#endif // TRADING_STUFF_MEM_H_
