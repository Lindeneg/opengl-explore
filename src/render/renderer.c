#include "renderer.h"

#include "../core/path.h"
#include "../math/math.h"
#include "gl.h"
#include "shader.h"
#include "texture.h"

// clang-format off
static const f32 CUBE_VERTS[] = {
    // pos x, y, z        uv
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // front (+z)
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // back (-z)
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // left (-x)
    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // right (+x)
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom (-y)
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, // top (+y)
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
};

static const u32 CUBE_INDICES[] = {
     0,  1,  2,   2,  3,  0,
     4,  5,  6,   6,  7,  4,
     8,  9, 10,  10, 11,  8,
    12, 13, 14,  14, 15, 12,
    16, 17, 18,  18, 19, 16,
    20, 21, 22,  22, 23, 20,
};
// clang-format on

void renderer_init(renderer_t *r, arena_t *scratch) {
    r->program = shader_from_files(scratch, FPATH(scratch, "assets", "shaders", "cube.vert"),
                                   FPATH(scratch, "assets", "shaders", "cube.frag"));
    ASSERT_MSG(r->program, "failed to build cube shader program");
    r->u_mvp = glGetUniformLocation(r->program, "uMVP");
    r->u_tex = glGetUniformLocation(r->program, "uTex");

    r->texture = texture_load(scratch, FPATH(scratch, "assets", "KayKit_City_Builder_Bits_1.0_FREE",
                                             "KayKit_City_Builder_Bits_1.0_FREE", "Assets", "gltf",
                                             "citybits_texture.png"));
    ASSERT_MSG(r->texture, "failed to load cube texture");

    glGenVertexArrays(1, &r->vao);
    glBindVertexArray(r->vao);

    glGenBuffers(1, &r->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, r->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CUBE_VERTS), CUBE_VERTS, GL_STATIC_DRAW);

    glGenBuffers(1, &r->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(CUBE_INDICES), CUBE_INDICES, GL_STATIC_DRAW);
    r->index_count = sizeof(CUBE_INDICES) / sizeof(CUBE_INDICES[0]);

    GLsizei stride = (GLsizei)(5 * sizeof(f32));
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (const void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (const void *)(3 * sizeof(f32)));
    glEnableVertexAttribArray(1);

    // The VAO records the GL_ELEMENT_ARRAY_BUFFER binding, so unbind the VAO first.
    glBindVertexArray(0);

    glUseProgram(r->program);
    glUniform1i(r->u_tex, 0);

    glEnable(GL_DEPTH_TEST);
}

void renderer_draw(renderer_t *r, i32 fb_w, i32 fb_h, f64 t) {
    glViewport(0, 0, fb_w, fb_h);
    f32 aspect = fb_h > 0 ? (f32)fb_w / (f32)fb_h : 1.0f;

    mat4_t model = mat4_rotate(vec3(0.5f, 1.0f, 0.0f), (f32)t);
    mat4_t view = mat4_look_at(vec3(0.0f, 0.0f, 4.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    mat4_t proj = mat4_perspective(deg2rad(60.0f), aspect, 0.1f, 100.0f);
    mat4_t mvp = mat4_mul(proj, mat4_mul(view, model));

    glClearColor(0.10f, 0.12f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(r->program);
    glUniformMatrix4fv(r->u_mvp, 1, GL_FALSE, mvp.e);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, r->texture);
    glBindVertexArray(r->vao);
    glDrawElements(GL_TRIANGLES, (GLsizei)r->index_count, GL_UNSIGNED_INT, (const void *)0);
}

void renderer_shutdown(renderer_t *r) {
    glDeleteTextures(1, &r->texture);
    glDeleteBuffers(1, &r->vbo);
    glDeleteBuffers(1, &r->ebo);
    glDeleteVertexArrays(1, &r->vao);
    glDeleteProgram(r->program);
    *r = (renderer_t){0};
}
