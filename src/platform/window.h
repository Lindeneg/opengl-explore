#ifndef TRADING_STUFF_WINDOW_H_
#define TRADING_STUFF_WINDOW_H_

#include "../common.h"

typedef struct {
    void *handle;
} window_t;

window_t window_create(i32 width, i32 height, const char *title);
void window_destroy(window_t *w);

b32 window_should_close(window_t *w);
void window_poll(void);
void window_swap(window_t *w);

f64 window_time(void);
void window_framebuffer_size(window_t *w, i32 *out_w, i32 *out_h);

void_fn window_get_proc(const char *name);

#endif // TRADING_STUFF_WINDOW_H_
