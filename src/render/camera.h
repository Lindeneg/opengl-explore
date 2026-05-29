#ifndef TRADING_STUFF_CAMERA_H_
#define TRADING_STUFF_CAMERA_H_

#include "../common.h"
#include "../math/math.h"

// Fixed-orientation RTS camera: looks at a ground focus from a fixed pitch.
typedef struct {
    vec3_t focus;
    f32 distance;
    f32 pitch; // radians above the horizontal
} camera_t;

camera_t camera_make(vec3_t focus, f32 distance, f32 pitch_rad);
void camera_pan(camera_t *c, f32 dx, f32 dz);
void camera_zoom(camera_t *c, f32 delta); // positive = zoom in (closer)

mat4_t camera_view(const camera_t *c);
mat4_t camera_proj(const camera_t *c, f32 aspect);

#endif // TRADING_STUFF_CAMERA_H_
