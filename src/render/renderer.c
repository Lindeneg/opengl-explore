#include "renderer.h"

#include "../core/log.h"
#include "gl.h"
#include "shader.h"

// clang-format off
static const f32 TRIANGLE[] = {
    // pos x, y     color r, g, b
    -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,
     0.5f, -0.5f,   0.0f, 1.0f, 0.0f,
     0.0f,  0.5f,   0.0f, 0.0f, 1.0f,
};
// clang-format on

static const char *VS_SRC = "#version 430 core\n"
                            "layout(location = 0) in vec2 aPos;\n"
                            "layout(location = 1) in vec3 aColor;\n"
                            "out vec3 vColor;\n"
                            "void main() {\n"
                            "    vColor = aColor;\n"
                            "    gl_Position = vec4(aPos, 0.0, 1.0);\n"
                            "}\n";

static const char *FS_SRC = "#version 430 core\n"
                            "in vec3 vColor;\n"
                            "out vec4 FragColor;\n"
                            "void main() {\n"
                            "    FragColor = vec4(vColor, 1.0);\n"
                            "}\n";

void renderer_init(renderer_t *r, arena_t *scratch) {
    r->program = shader_from_src(scratch, VS_SRC, FS_SRC);
    ASSERT_MSG(r->program, "failed to build triangle shader program");

    glGenVertexArrays(1, &r->vao);
    glBindVertexArray(r->vao);

    glGenBuffers(1, &r->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, r->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TRIANGLE), TRIANGLE, GL_STATIC_DRAW);

    GLsizei stride = (GLsizei)(5 * sizeof(f32));
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (const void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (const void *)(2 * sizeof(f32)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void renderer_draw(renderer_t *r) {
    glClearColor(0.10f, 0.12f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(r->program);
    glBindVertexArray(r->vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void renderer_shutdown(renderer_t *r) {
    glDeleteBuffers(1, &r->vbo);
    glDeleteVertexArrays(1, &r->vao);
    glDeleteProgram(r->program);
    *r = (renderer_t){0};
}
