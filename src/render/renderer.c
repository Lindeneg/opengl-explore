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

void renderer_draw(renderer_t *r, i32 fb_w, i32 fb_h, const mat4_t *view_proj) {
    glViewport(0, 0, fb_w, fb_h);
    glClearColor(0.10f, 0.12f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(r->program);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, r->texture);
    glBindVertexArray(r->mesh.vao);

    // Placeholder grid until we have a real scene/object list.
    const i32 grid = 7;
    const f32 spacing = 4.0f;
    f32 origin = -0.5f * (f32)(grid - 1) * spacing;
    for (i32 gz = 0; gz < grid; ++gz) {
        for (i32 gx = 0; gx < grid; ++gx) {
            vec3_t pos = vec3(origin + (f32)gx * spacing, 0.0f, origin + (f32)gz * spacing);
            mat4_t mvp = mat4_mul(*view_proj, mat4_translate(pos));
            glUniformMatrix4fv(r->u_mvp, 1, GL_FALSE, mvp.e);
            glDrawElements(GL_TRIANGLES, (GLsizei)r->mesh.index_count, GL_UNSIGNED_INT,
                           (const void *)0);
        }
    }
}

void renderer_shutdown(renderer_t *r) {
    mesh_destroy(&r->mesh);
    glDeleteTextures(1, &r->texture);
    glDeleteProgram(r->program);
    *r = (renderer_t){0};
}
