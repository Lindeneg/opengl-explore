#include "renderer.h"

#include "../core/path.h"
#include "../math/math.h"
#include "gl.h"
#include "mesh.h"
#include "shader.h"
#include "texture.h"

void renderer_init(renderer_t *r, arena_t *scratch) {
    r->program = shader_from_files(scratch, FPATH(scratch, "assets", "shaders", "mesh.vert"),
                                   FPATH(scratch, "assets", "shaders", "mesh.frag"));
    ASSERT_MSG(r->program, "failed to build mesh shader program");
    r->u_mvp = glGetUniformLocation(r->program, "uMVP");
    r->u_tex = glGetUniformLocation(r->program, "uTex");

    r->texture = texture_load(scratch, FPATH(scratch, "assets", "KayKit_City_Builder_Bits_1.0_FREE",
                                             "KayKit_City_Builder_Bits_1.0_FREE", "Assets", "gltf",
                                             "citybits_texture.png"));
    ASSERT_MSG(r->texture, "failed to load atlas texture");

    r->mesh = mesh_load_gltf(scratch, FPATH(scratch, "assets", "KayKit_City_Builder_Bits_1.0_FREE",
                                            "KayKit_City_Builder_Bits_1.0_FREE", "Assets", "gltf",
                                            "building_A.gltf"));

    glUseProgram(r->program);
    glUniform1i(r->u_tex, 0);

    glEnable(GL_DEPTH_TEST);
}

void renderer_draw(renderer_t *r, i32 fb_w, i32 fb_h, f64 t) {
    glViewport(0, 0, fb_w, fb_h);
    f32 aspect = fb_h > 0 ? (f32)fb_w / (f32)fb_h : 1.0f;

    mat4_t model = mat4_rotate(vec3(0.0f, 1.0f, 0.0f), (f32)t);
    mat4_t view = mat4_look_at(vec3(0.0f, 2.0f, 6.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    mat4_t proj = mat4_perspective(deg2rad(60.0f), aspect, 0.1f, 100.0f);
    mat4_t mvp = mat4_mul(proj, mat4_mul(view, model));

    glClearColor(0.10f, 0.12f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(r->program);
    glUniformMatrix4fv(r->u_mvp, 1, GL_FALSE, mvp.e);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, r->texture);
    glBindVertexArray(r->mesh.vao);
    glDrawElements(GL_TRIANGLES, (GLsizei)r->mesh.index_count, GL_UNSIGNED_INT, (const void *)0);
}

void renderer_shutdown(renderer_t *r) {
    mesh_destroy(&r->mesh);
    glDeleteTextures(1, &r->texture);
    glDeleteProgram(r->program);
    *r = (renderer_t){0};
}
