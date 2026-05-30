#ifndef TRADING_STUFF_LOG_H_
#define TRADING_STUFF_LOG_H_

#include "../common.h"

typedef enum {
    LL_TRACE,
    LL_DEBUG,
    LL_INFO,
    LL_WARN,
    LL_ERROR,
    LL_FATAL,
} log_level_t;

// file_path may be NULL for console-only output
void log_init(const char *file_path);
void log_shutdown(void);

void log_set_level(log_level_t min);

void log_write(log_level_t level, const char *file, i32 line, const char *fmt, ...);

#define LOG_TRACE(...) log_write(LL_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_DEBUG(...) log_write(LL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...) log_write(LL_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARN(...) log_write(LL_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...) log_write(LL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_FATAL(...) log_write(LL_FATAL, __FILE__, __LINE__, __VA_ARGS__)

#endif // TRADING_STUFF_LOG_H_
