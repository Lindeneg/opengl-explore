#if defined(__linux__)
#define _GNU_SOURCE // expose localtime_r / isatty under strict -std=c17
#endif

#include "log.h"

#include <stdarg.h>
#include <time.h>

#if PLATFORM_WINDOWS
#include <windows.h>
#elif PLATFORM_LINUX
#include <pthread.h>
#include <unistd.h>
#endif

#if PLATFORM_WINDOWS
typedef SRWLOCK os_mutex_t;
#define OS_MUTEX_INIT SRWLOCK_INIT
static void os_lock(os_mutex_t *m) { AcquireSRWLockExclusive(m); }
static void os_unlock(os_mutex_t *m) { ReleaseSRWLockExclusive(m); }
#elif PLATFORM_LINUX
typedef pthread_mutex_t os_mutex_t;
#define OS_MUTEX_INIT PTHREAD_MUTEX_INITIALIZER
static void os_lock(os_mutex_t *m) { pthread_mutex_lock(m); }
static void os_unlock(os_mutex_t *m) { pthread_mutex_unlock(m); }
#endif

static FILE *os_fopen(const char *path, const char *mode) {
#if PLATFORM_WINDOWS
    FILE *f = NULL;
    fopen_s(&f, path, mode);
    return f;
#elif PLATFORM_LINUX
    return fopen(path, mode);
#endif
}

static void os_localtime(struct tm *out, const time_t *t) {
#if PLATFORM_WINDOWS
    localtime_s(out, t); // MSVC arg order: (dst, src)
#elif PLATFORM_LINUX
    localtime_r(t, out); // POSIX arg order: (src, dst)
#endif
}

static b32 os_enable_color(void) {
#if PLATFORM_WINDOWS
    DWORD mode = 0;
    HANDLE err = GetStdHandle(STD_ERROR_HANDLE);
    if (GetConsoleMode(err, &mode))
        SetConsoleMode(err, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    if (!GetConsoleMode(out, &mode))
        return false; // redirected to a file/pipe: skip colors
    SetConsoleMode(out, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    return true;
#elif PLATFORM_LINUX
    return isatty(STDOUT_FILENO); // terminals handle ANSI natively
#endif
}

static os_mutex_t g_lock = OS_MUTEX_INIT;
static FILE *g_file = NULL;
static log_level_t g_min = LOG_TRACE;
static b32 g_color = false; // true when stdout is a console that understands ANSI

static const char *level_name(log_level_t level) {
    switch (level) {
    case LOG_TRACE:
        return "TRACE";
    case LOG_DEBUG:
        return "DEBUG";
    case LOG_INFO:
        return "INFO";
    case LOG_WARN:
        return "WARN";
    case LOG_ERROR:
        return "ERROR";
    case LOG_FATAL:
        return "FATAL";
    }
    return "?????";
}

static const char *level_color(log_level_t level) {
    switch (level) {
    case LOG_TRACE:
        return "\x1b[90m"; // bright black
    case LOG_DEBUG:
        return "\x1b[36m"; // cyan
    case LOG_INFO:
        return "\x1b[32m"; // green
    case LOG_WARN:
        return "\x1b[33m"; // yellow
    case LOG_ERROR:
        return "\x1b[31m"; // red
    case LOG_FATAL:
        return "\x1b[35m"; // magenta
    }
    return "";
}

void log_init(const char *file_path) {
    os_lock(&g_lock);
    if (file_path && !g_file)
        g_file = os_fopen(file_path, "w");
    g_color = os_enable_color();
    os_unlock(&g_lock);
}

void log_shutdown(void) {
    os_lock(&g_lock);
    if (g_file) {
        fclose(g_file);
        g_file = NULL;
    }
    os_unlock(&g_lock);
}

void log_set_level(log_level_t min) { g_min = min; }

void log_write(log_level_t level, const char *file, i32 line, const char *fmt, ...) {
    if (level < g_min)
        return;

    // extract file name
    const char *name = file;
    for (const char *p = file; *p; ++p) {
        if (*p == '/' || *p == '\\')
            name = p + 1;
    }

    char ts[16];
    time_t now = time(NULL);
    struct tm lt;
    os_localtime(&lt, &now);
    strftime(ts, sizeof(ts), "%H:%M:%S", &lt);

    char msg[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msg, sizeof(msg), fmt, args);
    va_end(args);

    const char *lvl = level_name(level);
    const char *color = g_color ? level_color(level) : "";
    const char *reset = g_color ? "\x1b[0m" : "";
    FILE *console = (level >= LOG_ERROR) ? stderr : stdout;

    os_lock(&g_lock);

    fprintf(console, "%s %s%-5s%s %s:%d: %s\n", ts, color, lvl, reset, name, line, msg);

    if (g_file) {
        fprintf(g_file, "%s %-5s %s:%d: %s\n", ts, lvl, name, line, msg);
        fflush(g_file);
    }

#if PLATFORM_WINDOWS
    char dbg[1100];
    snprintf(dbg, sizeof(dbg), "[%s] %s:%d: %s\n", lvl, name, line, msg);
    OutputDebugStringA(dbg);
#endif

    os_unlock(&g_lock);

    if (level == LOG_FATAL) {
        fflush(NULL); // flush every stream before we die
        abort();
    }
}
