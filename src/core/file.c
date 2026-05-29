#if defined(__linux__)
#define _GNU_SOURCE
#endif

#include "file.h"

#include "log.h"

static FILE *os_fopen(const char *path, const char *mode) {
#if PLATFORM_WINDOWS
    FILE *f = NULL;
    fopen_s(&f, path, mode);
    return f;
#elif PLATFORM_LINUX
    return fopen(path, mode);
#endif
}

file_data_t file_read(arena_t *a, const char *path) {
    file_data_t result = {0};

    FILE *f = os_fopen(path, "rb");
    if (!f) {
        LOG_ERROR("file_read: cannot open %s", path);
        return result;
    }

    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (len < 0) {
        LOG_ERROR("file_read: cannot size %s", path);
        fclose(f);
        return result;
    }

    u8 *buf = push_array(a, u8, (u64)len + 1);
    u64 got = (u64)fread(buf, 1, (u64)len, f);
    fclose(f);

    if (got != (u64)len) {
        LOG_ERROR("file_read: short read on %s (%llu of %ld)", path, (unsigned long long)got, len);
        return result;
    }

    buf[len] = 0;
    result.data = buf;
    result.size = (u64)len;
    return result;
}
