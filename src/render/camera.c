#include "camera.h"

#define CAMERA_DIST_MIN 5.0f
#define CAMERA_DIST_MAX 80.0f

camera_t camera_make(vec3_t focus, f32 distance, f32 pitch_rad) {
    return (camera_t){.focus = focus, .distance = distance, .pitch = pitch_rad};
}

void camera_pan(camera_t *c, f32 dx, f32 dz) {
    c->focus.x += dx;
    c->focus.z += dz;
}

void camera_zoom(camera_t *c, f32 delta) {
    c->distance -= delta;
    if (c->distance < CAMERA_DIST_MIN)
        c->distance = CAMERA_DIST_MIN;
    if (c->distance > CAMERA_DIST_MAX)
        c->distance = CAMERA_DIST_MAX;
}

mat4_t camera_view(const camera_t *c) {
    vec3_t offset = vec3(0.0f, sinf(c->pitch) * c->distance, cosf(c->pitch) * c->distance);
    vec3_t eye = vec3_add(c->focus, offset);
    return mat4_look_at(eye, c->focus, vec3(0.0f, 1.0f, 0.0f));
}

mat4_t camera_proj(const camera_t *c, f32 aspect) {
    (void)c;
    return mat4_perspective(deg2rad(50.0f), aspect, 0.1f, 1000.0f);
}
