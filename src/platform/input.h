#ifndef TRADING_STUFF_INPUT_H_
#define TRADING_STUFF_INPUT_H_

#include "../common.h"
#include "window.h"

typedef enum {
    KEY_W,
    KEY_A,
    KEY_S,
    KEY_D,
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_ESCAPE,
    KEY_COUNT,
} key_t;

void input_init(window_t *w);

b32 input_key_down(key_t k);
void input_mouse(f32 *x, f32 *y); // normalized 0..1, top-left origin
f32 input_scroll_delta(void);     // accumulated wheel since last call, then reset

#endif // TRADING_STUFF_INPUT_H_
