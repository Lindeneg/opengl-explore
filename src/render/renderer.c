#include "renderer.h"

#include "../core/path.h"
#include "../math/math.h"
#include "gl.h"
#include "shader.h"

void renderer_init(renderer_t *r, arena_t *scratch) {
    *r = (renderer_t){0};

    r->program = shader_from_files(scratch, FPATH(scratch, "assets", "shaders", "mesh.vert"),
                                   FPATH(scratch, "assets", "shaders", "mesh.frag"));
    ASSERT_MSG(r->program, "failed to build mesh shader program");
    r->u_mvp = glGetUniformLocation(r->program, "uMVP");
    r->u_tex = glGetUniformLocation(r->program, "uTex");

    r->color_program = shader_from_files(scratch, FPATH(scratch, "assets", "shaders", "color.vert"),
                                         FPATH(scratch, "assets", "shaders", "color.frag"));
    ASSERT_MSG(r->color_program, "failed to build color shader program");
    r->c_mvp = glGetUniformLocation(r->color_program, "uMVP");
    r->c_color = glGetUniformLocation(r->color_program, "uColor");

    r->quad = mesh_make_quad(1.0f);
    glEnable(GL_DEPTH_TEST);
}

void renderer_begin(renderer_t *r, i32 fb_w, i32 fb_h, const mat4_t *view_proj) {
    r->view_proj = *view_proj;

    glViewport(0, 0, fb_w, fb_h);
    glClearColor(0.10f, 0.12f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    r->bound_texture = 0;  // force texture bind on first textured draw
    r->active_program = 0; // force glUseProgram on first draw of either kind
}

static void use_program(renderer_t *r, u32 program) {
    if (r->active_program == program)
        return;
    glUseProgram(program);
    r->active_program = program;
    if (program == r->program) {
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(r->u_tex, 0);
    }
}

void renderer_draw(renderer_t *r, const mesh_t *mesh, const mat4_t *model, u32 texture) {
    use_program(r, r->program);
    if (texture != r->bound_texture) {
        glBindTexture(GL_TEXTURE_2D, texture);
        r->bound_texture = texture;
    }
    mat4_t mvp = mat4_mul(r->view_proj, *model);
    glUniformMatrix4fv(r->u_mvp, 1, GL_FALSE, mvp.e);
    glBindVertexArray(mesh->vao);
    glDrawElements(GL_TRIANGLES, (GLsizei)mesh->index_count, GL_UNSIGNED_INT, (const void *)0);
}

void renderer_fill_quad(renderer_t *r, const mat4_t *model, vec4_t color) {
    use_program(r, r->color_program);
    mat4_t mvp = mat4_mul(r->view_proj, *model);
    glUniformMatrix4fv(r->c_mvp, 1, GL_FALSE, mvp.e);
    glUniform4f(r->c_color, color.x, color.y, color.z, color.w);
    glBindVertexArray(r->quad.vao);
    glDrawElements(GL_TRIANGLES, (GLsizei)r->quad.index_count, GL_UNSIGNED_INT, (const void *)0);
}

void renderer_shutdown(renderer_t *r) {
    mesh_destroy(&r->quad);
    glDeleteProgram(r->program);
    glDeleteProgram(r->color_program);
    *r = (renderer_t){0};
}
