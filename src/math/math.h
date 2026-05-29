#ifndef TRADING_STUFF_MATH_H_
#define TRADING_STUFF_MATH_H_

#include <math.h>

#include "../common.h"

#define PI 3.14159265358979323846f

typedef struct {
    f32 x, y;
} vec2_t;

typedef struct {
    f32 x, y, z;
} vec3_t;

typedef struct {
    f32 x, y, z, w;
} vec4_t;

typedef struct {
    f32 e[16]; // column-major
} mat4_t;

static inline f32 deg2rad(f32 deg) { return deg * (PI / 180.0f); }

static inline vec3_t vec3(f32 x, f32 y, f32 z) { return (vec3_t){x, y, z}; }

static inline vec3_t vec3_add(vec3_t a, vec3_t b) {
    return (vec3_t){a.x + b.x, a.y + b.y, a.z + b.z};
}
static inline vec3_t vec3_sub(vec3_t a, vec3_t b) {
    return (vec3_t){a.x - b.x, a.y - b.y, a.z - b.z};
}
static inline vec3_t vec3_scale(vec3_t v, f32 s) { return (vec3_t){v.x * s, v.y * s, v.z * s}; }
static inline f32 vec3_dot(vec3_t a, vec3_t b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
static inline vec3_t vec3_cross(vec3_t a, vec3_t b) {
    return (vec3_t){a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}
static inline f32 vec3_length(vec3_t v) { return sqrtf(vec3_dot(v, v)); }
static inline vec3_t vec3_normalize(vec3_t v) {
    f32 len = vec3_length(v);
    return len > 0.0f ? vec3_scale(v, 1.0f / len) : v;
}

mat4_t mat4_identity(void);
mat4_t mat4_mul(mat4_t a, mat4_t b);
mat4_t mat4_translate(vec3_t t);
mat4_t mat4_scale(vec3_t s);
mat4_t mat4_rotate(vec3_t axis, f32 radians);
mat4_t mat4_perspective(f32 fovy_radians, f32 aspect, f32 znear, f32 zfar);
mat4_t mat4_look_at(vec3_t eye, vec3_t center, vec3_t up);

#endif // TRADING_STUFF_MATH_H_
