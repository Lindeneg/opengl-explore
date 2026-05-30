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

typedef enum {
    MOUSE_LEFT,
    MOUSE_RIGHT,
    MOUSE_COUNT,
} mouse_button_t;

void input_init(window_t *w);
void input_update(void); // snapshot per-frame button edges; call once after window_poll

b32 input_key_down(key_t k);
void input_mouse(f32 *x, f32 *y); // normalized 0..1, top-left origin
f32 input_scroll_delta(void);     // accumulated wheel since last call, then reset

b32 input_mouse_down(mouse_button_t b);     // held this frame
b32 input_mouse_pressed(mouse_button_t b);  // went down this frame
b32 input_mouse_released(mouse_button_t b); // went up this frame

#endif // TRADING_STUFF_INPUT_H_
