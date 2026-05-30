#ifndef TRADING_STUFF_COMMON_H_
#define TRADING_STUFF_COMMON_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(_WIN32)
#define PLATFORM_WINDOWS 1
#define PLATFORM_LINUX 0
#elif defined(__linux__)
#define PLATFORM_WINDOWS 0
#define PLATFORM_LINUX 1
#else
#error "unsupported platform"
#endif

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef int8_t b8;
typedef int32_t b32;

#ifndef true
#define true 1
#define false 0
#endif

typedef void (*void_fn)(void); // generic function pointer (e.g. GL proc address)

#define KiB(n) ((u64)(n) << 10)
#define MiB(n) ((u64)(n) << 20)
#define GiB(n) ((u64)(n) << 30)

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define ALIGN_UP_2(n, p) (((u64)(n) + ((u64)(p) - 1)) & (~((u64)(p) - 1)))

void log_assert(b32 fatal, const char *file, i32 line, const char *fmt, ...);

#define ASSERT(expr)                                                                               \
    do {                                                                                           \
        if (!(expr))                                                                               \
            log_assert(true, __FILE__, __LINE__, "ASSERT '%s' failed (%s)", #expr, __func__);      \
    } while (0)

#define ASSERT_MSG(expr, fmt, ...)                                                                 \
    do {                                                                                           \
        if (!(expr))                                                                               \
            log_assert(true, __FILE__, __LINE__, "ASSERT '%s' failed: " fmt " (%s)", #expr,        \
                       ##__VA_ARGS__, __func__);                                                   \
    } while (0)

#define ASSERT_RET(expr, ret)                                                                      \
    do {                                                                                           \
        if (!(expr)) {                                                                             \
            log_assert(false, __FILE__, __LINE__, "check '%s' failed (%s)", #expr, __func__);      \
            return (ret);                                                                          \
        }                                                                                          \
    } while (0)

#define ASSERT_RET_V(expr)                                                                         \
    do {                                                                                           \
        if (!(expr)) {                                                                             \
            log_assert(false, __FILE__, __LINE__, "check '%s' failed (%s)", #expr, __func__);      \
            return;                                                                                \
        }                                                                                          \
    } while (0)

#define ASSERT_RET_MSG(expr, ret, fmt, ...)                                                        \
    do {                                                                                           \
        if (!(expr)) {                                                                             \
            log_assert(false, __FILE__, __LINE__, "check '%s' failed: " fmt " (%s)", #expr,        \
                       ##__VA_ARGS__, __func__);                                                   \
            return (ret);                                                                          \
        }                                                                                          \
    } while (0)

#define ASSERT_RET_V_MSG(expr, fmt, ...)                                                           \
    do {                                                                                           \
        if (!(expr)) {                                                                             \
            log_assert(false, __FILE__, __LINE__, "check '%s' failed: " fmt " (%s)", #expr,        \
                       ##__VA_ARGS__, __func__);                                                   \
            return;                                                                                \
        }                                                                                          \
    } while (0)

#endif // TRADING_STUFF_COMMON_H_
