#include "renderer.h"

#include "../core/path.h"
#include "../math/math.h"
#include "gl.h"
#include "shader.h"

void renderer_init(renderer_t *r, arena_t *scratch) {
    r->program = shader_from_files(scratch, FPATH(scratch, "assets", "shaders", "mesh.vert"),
                                   FPATH(scratch, "assets", "shaders", "mesh.frag"));
    ASSERT_MSG(r->program, "failed to build mesh shader program");
    r->u_mvp = glGetUniformLocation(r->program, "uMVP");
    r->u_tex = glGetUniformLocation(r->program, "uTex");
    glEnable(GL_DEPTH_TEST);
}

void renderer_begin(renderer_t *r, i32 fb_w, i32 fb_h, const mat4_t *view_proj) {
    r->view_proj = *view_proj;

    glViewport(0, 0, fb_w, fb_h);
    glClearColor(0.10f, 0.12f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(r->program);
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(r->u_tex, 0);
    r->bound_texture = 0; // force a bind on the first draw of the frame
}

void renderer_draw(renderer_t *r, const mesh_t *mesh, const mat4_t *model, u32 texture) {
    if (texture != r->bound_texture) {
        glBindTexture(GL_TEXTURE_2D, texture);
        r->bound_texture = texture;
    }
    mat4_t mvp = mat4_mul(r->view_proj, *model);
    glUniformMatrix4fv(r->u_mvp, 1, GL_FALSE, mvp.e);
    glBindVertexArray(mesh->vao);
    glDrawElements(GL_TRIANGLES, (GLsizei)mesh->index_count, GL_UNSIGNED_INT, (const void *)0);
}

void renderer_shutdown(renderer_t *r) {
    glDeleteProgram(r->program);
    *r = (renderer_t){0};
}
