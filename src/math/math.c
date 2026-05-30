#include "math.h"

mat4_t mat4_identity(void) {
    mat4_t m = {0};
    m.e[0] = m.e[5] = m.e[10] = m.e[15] = 1.0f;
    return m;
}

// C = A * B, column-major: C[col][row] = sum_k A[k][row] * B[col][k].
mat4_t mat4_mul(mat4_t a, mat4_t b) {
    mat4_t r;
    for (i32 col = 0; col < 4; ++col) {
        for (i32 row = 0; row < 4; ++row) {
            r.e[col * 4 + row] =
                a.e[0 * 4 + row] * b.e[col * 4 + 0] + a.e[1 * 4 + row] * b.e[col * 4 + 1] +
                a.e[2 * 4 + row] * b.e[col * 4 + 2] + a.e[3 * 4 + row] * b.e[col * 4 + 3];
        }
    }
    return r;
}

mat4_t mat4_translate(vec3_t t) {
    mat4_t m = mat4_identity();
    m.e[12] = t.x;
    m.e[13] = t.y;
    m.e[14] = t.z;
    return m;
}

mat4_t mat4_scale(vec3_t s) {
    mat4_t m = {0};
    m.e[0] = s.x;
    m.e[5] = s.y;
    m.e[10] = s.z;
    m.e[15] = 1.0f;
    return m;
}

mat4_t mat4_rotate(vec3_t axis, f32 radians) {
    vec3_t a = vec3_normalize(axis);
    f32 c = cosf(radians);
    f32 s = sinf(radians);
    f32 t = 1.0f - c;

    mat4_t m = {0};
    m.e[0] = t * a.x * a.x + c;
    m.e[1] = t * a.x * a.y + s * a.z;
    m.e[2] = t * a.x * a.z - s * a.y;
    m.e[4] = t * a.x * a.y - s * a.z;
    m.e[5] = t * a.y * a.y + c;
    m.e[6] = t * a.y * a.z + s * a.x;
    m.e[8] = t * a.x * a.z + s * a.y;
    m.e[9] = t * a.y * a.z - s * a.x;
    m.e[10] = t * a.z * a.z + c;
    m.e[15] = 1.0f;
    return m;
}

mat4_t mat4_perspective(f32 fovy_radians, f32 aspect, f32 znear, f32 zfar) {
    f32 f = 1.0f / tanf(fovy_radians * 0.5f);
    mat4_t m = {0};
    m.e[0] = f / aspect;
    m.e[5] = f;
    m.e[10] = (zfar + znear) / (znear - zfar);
    m.e[11] = -1.0f;
    m.e[14] = (2.0f * zfar * znear) / (znear - zfar);
    return m;
}

mat4_t mat4_look_at(vec3_t eye, vec3_t center, vec3_t up) {
    vec3_t f = vec3_normalize(vec3_sub(center, eye));
    vec3_t s = vec3_normalize(vec3_cross(f, up));
    vec3_t u = vec3_cross(s, f);

    mat4_t m = {0};
    m.e[0] = s.x;
    m.e[4] = s.y;
    m.e[8] = s.z;
    m.e[1] = u.x;
    m.e[5] = u.y;
    m.e[9] = u.z;
    m.e[2] = -f.x;
    m.e[6] = -f.y;
    m.e[10] = -f.z;
    m.e[12] = -vec3_dot(s, eye);
    m.e[13] = -vec3_dot(u, eye);
    m.e[14] = vec3_dot(f, eye);
    m.e[15] = 1.0f;
    return m;
}

// Cofactor inverse over the flat column-major array (the classic MESA formulation).
mat4_t mat4_inverse(mat4_t m) {
    const f32 *e = m.e;
    f32 inv[16];
    inv[0] = e[5] * e[10] * e[15] - e[5] * e[11] * e[14] - e[9] * e[6] * e[15] +
             e[9] * e[7] * e[14] + e[13] * e[6] * e[11] - e[13] * e[7] * e[10];
    inv[4] = -e[4] * e[10] * e[15] + e[4] * e[11] * e[14] + e[8] * e[6] * e[15] -
             e[8] * e[7] * e[14] - e[12] * e[6] * e[11] + e[12] * e[7] * e[10];
    inv[8] = e[4] * e[9] * e[15] - e[4] * e[11] * e[13] - e[8] * e[5] * e[15] +
             e[8] * e[7] * e[13] + e[12] * e[5] * e[11] - e[12] * e[7] * e[9];
    inv[12] = -e[4] * e[9] * e[14] + e[4] * e[10] * e[13] + e[8] * e[5] * e[14] -
              e[8] * e[6] * e[13] - e[12] * e[5] * e[10] + e[12] * e[6] * e[9];
    inv[1] = -e[1] * e[10] * e[15] + e[1] * e[11] * e[14] + e[9] * e[2] * e[15] -
             e[9] * e[3] * e[14] - e[13] * e[2] * e[11] + e[13] * e[3] * e[10];
    inv[5] = e[0] * e[10] * e[15] - e[0] * e[11] * e[14] - e[8] * e[2] * e[15] +
             e[8] * e[3] * e[14] + e[12] * e[2] * e[11] - e[12] * e[3] * e[10];
    inv[9] = -e[0] * e[9] * e[15] + e[0] * e[11] * e[13] + e[8] * e[1] * e[15] -
             e[8] * e[3] * e[13] - e[12] * e[1] * e[11] + e[12] * e[3] * e[9];
    inv[13] = e[0] * e[9] * e[14] - e[0] * e[10] * e[13] - e[8] * e[1] * e[14] +
              e[8] * e[2] * e[13] + e[12] * e[1] * e[10] - e[12] * e[2] * e[9];
    inv[2] = e[1] * e[6] * e[15] - e[1] * e[7] * e[14] - e[5] * e[2] * e[15] + e[5] * e[3] * e[14] +
             e[13] * e[2] * e[7] - e[13] * e[3] * e[6];
    inv[6] = -e[0] * e[6] * e[15] + e[0] * e[7] * e[14] + e[4] * e[2] * e[15] -
             e[4] * e[3] * e[14] - e[12] * e[2] * e[7] + e[12] * e[3] * e[6];
    inv[10] = e[0] * e[5] * e[15] - e[0] * e[7] * e[13] - e[4] * e[1] * e[15] +
              e[4] * e[3] * e[13] + e[12] * e[1] * e[7] - e[12] * e[3] * e[5];
    inv[14] = -e[0] * e[5] * e[14] + e[0] * e[6] * e[13] + e[4] * e[1] * e[14] -
              e[4] * e[2] * e[13] - e[12] * e[1] * e[6] + e[12] * e[2] * e[5];
    inv[3] = -e[1] * e[6] * e[11] + e[1] * e[7] * e[10] + e[5] * e[2] * e[11] -
             e[5] * e[3] * e[10] - e[9] * e[2] * e[7] + e[9] * e[3] * e[6];
    inv[7] = e[0] * e[6] * e[11] - e[0] * e[7] * e[10] - e[4] * e[2] * e[11] + e[4] * e[3] * e[10] +
             e[8] * e[2] * e[7] - e[8] * e[3] * e[6];
    inv[11] = -e[0] * e[5] * e[11] + e[0] * e[7] * e[9] + e[4] * e[1] * e[11] - e[4] * e[3] * e[9] -
              e[8] * e[1] * e[7] + e[8] * e[3] * e[5];
    inv[15] = e[0] * e[5] * e[10] - e[0] * e[6] * e[9] - e[4] * e[1] * e[10] + e[4] * e[2] * e[9] +
              e[8] * e[1] * e[6] - e[8] * e[2] * e[5];

    f32 det = e[0] * inv[0] + e[1] * inv[4] + e[2] * inv[8] + e[3] * inv[12];
    ASSERT_RET_MSG(fabsf(det) > 1e-12f, mat4_identity(), "mat4_inverse: singular matrix");

    f32 inv_det = 1.0f / det;
    mat4_t r;
    for (i32 i = 0; i < 16; ++i)
        r.e[i] = inv[i] * inv_det;
    return r;
}
