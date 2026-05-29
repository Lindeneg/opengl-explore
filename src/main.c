#include "core/log.h"
#include "core/mem.h"
#include "platform/window.h"
#include "render/gl.h"
#include "render/renderer.h"

int main(void) {
    log_init(NULL); // console-only for now
    log_set_level(LOG_TRACE);
    LOG_INFO("app starting");

    arena_sys_init(MiB(256));
    arena_t frame = arena_create(MiB(16)); // per-frame scratch (and shader logs at init)

    window_t win = window_create(1280, 720, "tradingstuff");
    ASSERT_MSG(gl_load(window_get_proc), "failed to load OpenGL functions");
    gl_enable_debug_output();
    LOG_INFO("OpenGL context ready");

    renderer_t r;
    renderer_init(&r, &frame);

    while (!window_should_close(&win)) {
        window_poll();
        arena_reset(&frame);
        renderer_draw(&r);
        window_swap(&win);
    }

    renderer_shutdown(&r);
    window_destroy(&win);
    log_shutdown();
    return 0;
}
