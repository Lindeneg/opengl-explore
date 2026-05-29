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

typedef void (*void_fn)(void); // generic function pointer (e.g. a GL proc address)

#define KiB(n) ((u64)(n) << 10)
#define MiB(n) ((u64)(n) << 20)
#define GiB(n) ((u64)(n) << 30)

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define ALIGN_UP_2(n, p) (((u64)(n) + ((u64)(p) - 1)) & (~((u64)(p) - 1)))

#define ASSERT(expr)                                                                               \
    do {                                                                                           \
        if (!(expr)) {                                                                             \
            fprintf(stderr, "[ASSERT ERROR] [%s:%d:%s] Expression '%s' was false\n", __FILE__,     \
                    __LINE__, __func__, #expr);                                                    \
            abort();                                                                               \
        }                                                                                          \
    } while (0)

#define ASSERT_MSG(expr, fmt, ...)                                                                 \
    do {                                                                                           \
        if (!(expr)) {                                                                             \
            fprintf(stderr, "[ASSERT ERROR] [%s:%d:%s] " fmt "\n", __FILE__, __LINE__, __func__,   \
                    ##__VA_ARGS__);                                                                \
            abort();                                                                               \
        }                                                                                          \
    } while (0)

#define ASSERT_RET(expr, ret)                                                                      \
    do {                                                                                           \
        if (!(expr)) {                                                                             \
            fprintf(stderr, "[ASSERT WARNING] [%s:%d:%s] Expression '%s' was false\n", __FILE__,   \
                    __LINE__, __func__, #expr);                                                    \
            return (ret);                                                                          \
        }                                                                                          \
    } while (0)

#define ASSERT_RET_V(expr)                                                                         \
    do {                                                                                           \
        if (!(expr)) {                                                                             \
            fprintf(stderr, "[ASSERT WARNING] [%s:%d:%s] Expression '%s' was false\n", __FILE__,   \
                    __LINE__, __func__, #expr);                                                    \
            return;                                                                                \
        }                                                                                          \
    } while (0)

#define ASSERT_RET_MSG(expr, ret, fmt, ...)                                                        \
    do {                                                                                           \
        if (!(expr)) {                                                                             \
            fprintf(stderr, "[ASSERT WARNING] [%s:%d:%s] " fmt "\n", __FILE__, __LINE__, __func__, \
                    ##__VA_ARGS__);                                                                \
            return (ret);                                                                          \
        }                                                                                          \
    } while (0)

#define ASSERT_RET_V_MSG(expr, fmt, ...)                                                           \
    do {                                                                                           \
        if (!(expr)) {                                                                             \
            fprintf(stderr, "[ASSERT WARNING] [%s:%d:%s] " fmt "\n", __FILE__, __LINE__, __func__, \
                    ##__VA_ARGS__);                                                                \
            return;                                                                                \
        }                                                                                          \
    } while (0)

#endif // TRADING_STUFF_COMMON_H_
