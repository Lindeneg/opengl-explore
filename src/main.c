#include "core/log.h"
#include "core/mem.h"
#include "core/path.h"
#include "game/build.h"
#include "game/level.h"
#include "game/world.h"
#include "math/math.h"
#include "platform/input.h"
#include "platform/window.h"
#include "render/assets.h"
#include "render/camera.h"
#include "render/gl.h"
#include "render/renderer.h"

int main(void) {
    log_init(NULL);
    log_set_level(LOG_DEBUG);
    LOG_INFO("app starting");

    arena_sys_init(MiB(512));
    arena_t permanent = arena_create(MiB(256));
    arena_t frame = arena_create(MiB(64));

    window_t win = window_create(1280, 720, "tradingstuff");
    ASSERT_MSG(gl_load(window_get_proc), "failed to load OpenGL functions");
    gl_enable_debug_output();
    input_init(&win);
    LOG_INFO("OpenGL context ready");

    renderer_t r;
    renderer_init(&r, &frame);

    assets_t assets;
    assets_init(&assets, &permanent);

    const char *level_path = FPATH(&frame, "assets", "levels", "starter.level");
    level_t *level = level_load(&frame, level_path);
    if (!level) {
        LOG_INFO("no level at %s; generating one", level_path);
        level = level_generate(&frame, 1337);
        level_save(level, level_path);
    }
    world_t world = world_from_level(level, &assets, &permanent, &frame);

    camera_t cam = camera_make(vec3(0.0f, 0.0f, 0.0f), 40.0f, deg2rad(60.0f));
    build_tool_t tool = {0};

    f64 last = window_time();
    while (!window_should_close(&win)) {
        window_poll();
        input_update();
        arena_reset(&frame);

        f64 now = window_time();
        f32 dt = (f32)(now - last);
        last = now;

        if (input_key_down(KEY_ESCAPE))
            break;

        f32 px = 0.0f, pz = 0.0f;
        if (input_key_down(KEY_W) || input_key_down(KEY_UP))
            pz -= 1.0f;
        if (input_key_down(KEY_S) || input_key_down(KEY_DOWN))
            pz += 1.0f;
        if (input_key_down(KEY_A) || input_key_down(KEY_LEFT))
            px -= 1.0f;
        if (input_key_down(KEY_D) || input_key_down(KEY_RIGHT))
            px += 1.0f;

        f32 mx, my;
        input_mouse(&mx, &my);
        const f32 edge = 0.02f;
        if (mx < edge)
            px -= 1.0f;
        if (mx > 1.0f - edge)
            px += 1.0f;
        if (my < edge)
            pz -= 1.0f;
        if (my > 1.0f - edge)
            pz += 1.0f;

        f32 pan = dt * cam.distance;
        camera_pan(&cam, px * pan, pz * pan);
        camera_zoom(&cam, input_scroll_delta() * 2.0f);

        i32 fb_w, fb_h;
        window_framebuffer_size(&win, &fb_w, &fb_h);
        f32 aspect = fb_h > 0 ? (f32)fb_w / (f32)fb_h : 1.0f;

        f32 ndc_x = mx * 2.0f - 1.0f, ndc_y = 1.0f - my * 2.0f;
        vec3_t ground;
        i32 cx = 0, cz = 0;
        b32 hover = camera_pick_ground(&cam, aspect, ndc_x, ndc_y, &ground) &&
                    world_world_to_cell(&world, ground, &cx, &cz);
        build_tool_update(&tool, &world, hover, cx, cz);

        mat4_t view_proj = mat4_mul(camera_proj(&cam, aspect), camera_view(&cam));

        renderer_begin(&r, fb_w, fb_h, &view_proj);
        world_draw(&world, &assets, &r);
        world_draw_paths(&world, &assets, &r);
        build_tool_draw(&tool, &world, &assets, &r);
        window_swap(&win);
    }

    renderer_shutdown(&r);
    window_destroy(&win);
    log_shutdown();
    return 0;
}
