#ifndef TRADING_STUFF_CAMERA_H_
#define TRADING_STUFF_CAMERA_H_

#include "../common.h"
#include "../math/math.h"

// fixed-orientation RTS camera, looks at a ground focus from a fixed pitch
typedef struct {
    vec3_t focus;
    f32 distance;
    f32 pitch; // radians above horizontal
} camera_t;

camera_t camera_make(vec3_t focus, f32 distance, f32 pitch_rad);
void camera_pan(camera_t *c, f32 dx, f32 dz);
void camera_zoom(camera_t *c, f32 delta); // positive zooms in (closer)

mat4_t camera_view(const camera_t *c);
mat4_t camera_proj(const camera_t *c, f32 aspect);

// ray through NDC point (x,y in [-1,1], y up) onto y=0 ground plane, false if parallel or behind
b32 camera_pick_ground(const camera_t *c, f32 aspect, f32 ndc_x, f32 ndc_y, vec3_t *out_world);

#endif // TRADING_STUFF_CAMERA_H_
