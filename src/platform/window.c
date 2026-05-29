#define GLFW_INCLUDE_NONE
#include "window.h"

#include <GLFW/glfw3.h>

#include "../core/log.h"

window_t window_create(i32 width, i32 height, const char *title) {
    ASSERT_MSG(glfwInit(), "glfwInit failed");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

    GLFWwindow *handle = glfwCreateWindow(width, height, title, NULL, NULL);
    ASSERT_MSG(handle, "glfwCreateWindow failed");

    glfwMakeContextCurrent(handle);
    glfwSwapInterval(1); // vsync

    return (window_t){.handle = handle};
}

void window_destroy(window_t *w) {
    if (w->handle)
        glfwDestroyWindow((GLFWwindow *)w->handle);
    glfwTerminate();
    w->handle = NULL;
}

b32 window_should_close(window_t *w) {
    return glfwWindowShouldClose((GLFWwindow *)w->handle) ? true : false;
}

void window_poll(void) { glfwPollEvents(); }

void window_swap(window_t *w) { glfwSwapBuffers((GLFWwindow *)w->handle); }

f64 window_time(void) { return glfwGetTime(); }

void window_framebuffer_size(window_t *w, i32 *out_w, i32 *out_h) {
    glfwGetFramebufferSize((GLFWwindow *)w->handle, out_w, out_h);
}

void_fn window_get_proc(const char *name) { return (void_fn)glfwGetProcAddress(name); }
