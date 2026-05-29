#ifndef TRADING_STUFF_WINDOW_H_
#define TRADING_STUFF_WINDOW_H_

#include "../common.h"

// `handle` is a GLFWwindow*, kept as void* so this header pulls
// in no GLFW types. Held by value like arena_t.
typedef struct {
    void *handle;
} window_t;

// Create a window with a 4.3 core debug GL context made current. Aborts on failure.
window_t window_create(i32 width, i32 height, const char *title);
void window_destroy(window_t *w);

b32 window_should_close(window_t *w);
void window_poll(void);
void window_swap(window_t *w);

// GL proc-address loader to hand to gl_load(); keeps GLFW contained to window.c.
void_fn window_get_proc(const char *name);

#endif // TRADING_STUFF_WINDOW_H_
