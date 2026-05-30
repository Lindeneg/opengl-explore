#define GLFW_INCLUDE_NONE
#include "input.h"

#include <GLFW/glfw3.h>

static GLFWwindow *g_window;
static f64 g_scroll_accum;
static b8 g_mouse_cur[MOUSE_COUNT];
static b8 g_mouse_prev[MOUSE_COUNT];

static const int KEY_TO_GLFW[KEY_COUNT] = {
    [KEY_W] = GLFW_KEY_W,       [KEY_A] = GLFW_KEY_A,         [KEY_S] = GLFW_KEY_S,
    [KEY_D] = GLFW_KEY_D,       [KEY_UP] = GLFW_KEY_UP,       [KEY_DOWN] = GLFW_KEY_DOWN,
    [KEY_LEFT] = GLFW_KEY_LEFT, [KEY_RIGHT] = GLFW_KEY_RIGHT, [KEY_ESCAPE] = GLFW_KEY_ESCAPE,
};

static const int MOUSE_TO_GLFW[MOUSE_COUNT] = {
    [MOUSE_LEFT] = GLFW_MOUSE_BUTTON_LEFT,
    [MOUSE_RIGHT] = GLFW_MOUSE_BUTTON_RIGHT,
};

static void scroll_callback(GLFWwindow *w, double xoff, double yoff) {
    (void)w;
    (void)xoff;
    g_scroll_accum += yoff;
}

void input_init(window_t *w) {
    g_window = (GLFWwindow *)w->handle;
    glfwSetScrollCallback(g_window, scroll_callback);
}

void input_update(void) {
    for (i32 i = 0; i < MOUSE_COUNT; ++i) {
        g_mouse_prev[i] = g_mouse_cur[i];
        g_mouse_cur[i] = glfwGetMouseButton(g_window, MOUSE_TO_GLFW[i]) == GLFW_PRESS;
    }
}

b32 input_key_down(key_t k) { return glfwGetKey(g_window, KEY_TO_GLFW[k]) == GLFW_PRESS; }

b32 input_mouse_down(mouse_button_t b) { return g_mouse_cur[b]; }
b32 input_mouse_pressed(mouse_button_t b) { return g_mouse_cur[b] && !g_mouse_prev[b]; }
b32 input_mouse_released(mouse_button_t b) { return !g_mouse_cur[b] && g_mouse_prev[b]; }

void input_mouse(f32 *x, f32 *y) {
    double cx, cy;
    glfwGetCursorPos(g_window, &cx, &cy);
    int ww, wh;
    glfwGetWindowSize(g_window, &ww, &wh);
    *x = ww > 0 ? (f32)(cx / ww) : 0.0f;
    *y = wh > 0 ? (f32)(cy / wh) : 0.0f;
}

f32 input_scroll_delta(void) {
    f32 d = (f32)g_scroll_accum;
    g_scroll_accum = 0.0;
    return d;
}
