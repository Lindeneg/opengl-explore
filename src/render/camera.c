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

// unproject NDC point at depth nz into world space (inv is inverse view_proj)
static vec3_t unproject(const mat4_t *inv, f32 nx, f32 ny, f32 nz) {
    const f32 *e = inv->e;
    f32 x = e[0] * nx + e[4] * ny + e[8] * nz + e[12];
    f32 y = e[1] * nx + e[5] * ny + e[9] * nz + e[13];
    f32 z = e[2] * nx + e[6] * ny + e[10] * nz + e[14];
    f32 w = e[3] * nx + e[7] * ny + e[11] * nz + e[15];
    f32 iw = fabsf(w) > 1e-6f ? 1.0f / w : 0.0f;
    return vec3(x * iw, y * iw, z * iw);
}

b32 camera_pick_ground(const camera_t *c, f32 aspect, f32 ndc_x, f32 ndc_y, vec3_t *out_world) {
    mat4_t inv = mat4_inverse(mat4_mul(camera_proj(c, aspect), camera_view(c)));
    vec3_t ray_near = unproject(&inv, ndc_x, ndc_y, -1.0f);
    vec3_t ray_far = unproject(&inv, ndc_x, ndc_y, 1.0f);
    vec3_t dir = vec3_sub(ray_far, ray_near);
    if (fabsf(dir.y) < 1e-6f)
        return false;
    f32 t = -ray_near.y / dir.y;
    if (t < 0.0f)
        return false;
    *out_world = vec3_add(ray_near, vec3_scale(dir, t));
    return true;
}
