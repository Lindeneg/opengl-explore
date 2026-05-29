#if defined(__linux__)
#define _GNU_SOURCE // expose MAP_ANONYMOUS under strict -std=c17
#endif

#include "mem.h"

#include <string.h>

#if PLATFORM_WINDOWS
#include <windows.h>
#elif PLATFORM_LINUX
#include <sys/mman.h>
#include <unistd.h>
#endif

static u64 os_page_size(void) {
#if PLATFORM_WINDOWS
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return si.dwPageSize;
#elif PLATFORM_LINUX
    return (u64)sysconf(_SC_PAGESIZE);
#endif
}

static u64 os_alloc_granularity(void) {
#if PLATFORM_WINDOWS
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return si.dwAllocationGranularity;
#elif PLATFORM_LINUX
    return (u64)sysconf(_SC_PAGESIZE);
#endif
}

static void *os_reserve(u64 size) {
#if PLATFORM_WINDOWS
    return VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
#elif PLATFORM_LINUX
    void *p = mmap(0, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    return (p == MAP_FAILED) ? NULL : p;
#endif
}

static b32 os_commit(void *ptr, u64 size) {
#if PLATFORM_WINDOWS
    return VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != NULL;
#elif PLATFORM_LINUX
    return mprotect(ptr, size, PROT_READ | PROT_WRITE) == 0;
#endif
}

typedef struct {
    u8 *base;
    u64 reserved;
    u64 carved;
    u64 page_size;
} arena_root_t;

static arena_root_t g_arena_root;

void arena_sys_init(u64 reserve) {
    ASSERT_MSG(!g_arena_root.base, "memory system already initialized");
    g_arena_root.page_size = os_page_size();

    reserve = ALIGN_UP_2(reserve, os_alloc_granularity());
    void *base = os_reserve(reserve);
    ASSERT_MSG(base, "reserve of %llu bytes failed", (unsigned long long)reserve);

    g_arena_root.base = (u8 *)base;
    g_arena_root.reserved = reserve;
    g_arena_root.carved = 0;
}

arena_t arena_create(u64 cap) {
    cap = ALIGN_UP_2(cap, g_arena_root.page_size);
    ASSERT_MSG(g_arena_root.carved + cap <= g_arena_root.reserved,
               "arena budget exhausted: want %llu, %llu left", (unsigned long long)cap,
               (unsigned long long)(g_arena_root.reserved - g_arena_root.carved));

    arena_t arena = {
        .base = g_arena_root.base + g_arena_root.carved,
        .cap = cap,
    };
    g_arena_root.carved += cap;

    return arena;
}

static void arena_commit_for(arena_t *a, u64 needed) {
    if (needed <= a->committed)
        return;

    u64 want = ALIGN_UP_2(needed, ARENA_COMMIT_CHUNK);
    if (want > a->cap)
        want = a->cap; // cap is page-aligned, so this stays page-aligned

    b32 ok = os_commit(a->base + a->committed, want - a->committed);
    ASSERT_MSG(ok, "commit of %llu bytes failed", (unsigned long long)(want - a->committed));

    a->committed = want;
}

void *arena_push_aligned(arena_t *a, u64 size, u64 align) {
    ASSERT_RET(a, NULL);                         // bad arena pointer: bail safely
    ASSERT(align && (align & (align - 1)) == 0); // not power of two: a code bug

    u64 pos = ALIGN_UP_2(a->used, align);
    u64 new_used = pos + size;
    ASSERT_RET_MSG(new_used <= a->cap, NULL, "arena exhausted: want %llu, cap %llu",
                   (unsigned long long)new_used, (unsigned long long)a->cap);

    arena_commit_for(a, new_used);
    a->used = new_used;
    return a->base + pos;
}

void *arena_push(arena_t *a, u64 size) { return arena_push_aligned(a, size, ARENA_DEFAULT_ALIGN); }

void *arena_push_zero(arena_t *a, u64 size, u64 align) {
    void *p = arena_push_aligned(a, size, align);
    if (p)
        memset(p, 0, size);
    return p;
}

void arena_reset(arena_t *a) {
    ASSERT_RET_V(a);
    a->used = 0;
}

temp_t temp_begin(arena_t *a) {
    ASSERT_RET(a, (temp_t){0});
    return (temp_t){.arena = a, .used = a->used};
}

void temp_end(temp_t t) {
    ASSERT_RET_V(t.arena);
    t.arena->used = t.used;
}
