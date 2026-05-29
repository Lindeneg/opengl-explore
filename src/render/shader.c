#include "shader.h"

#include "../core/file.h"
#include "../core/log.h"
#include "gl.h"

static u32 compile_stage(arena_t *scratch, GLenum stage, const char *src) {
    u32 id = glCreateShader(stage);
    glShaderSource(id, 1, &src, NULL);
    glCompileShader(id);

    GLint ok = 0;
    glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        GLint len = 0;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);
        if (len < 1)
            len = 1;
        temp_t t = temp_begin(scratch);
        char *log = push_array(scratch, char, (u64)len);
        glGetShaderInfoLog(id, len, NULL, log);
        LOG_ERROR("shader compile failed: %s", log);
        temp_end(t);
        glDeleteShader(id);
        return 0;
    }
    return id;
}

u32 shader_from_src(arena_t *scratch, const char *vs_src, const char *fs_src) {
    u32 vs = compile_stage(scratch, GL_VERTEX_SHADER, vs_src);
    u32 fs = compile_stage(scratch, GL_FRAGMENT_SHADER, fs_src);
    u32 program = 0;

    if (vs && fs) {
        program = glCreateProgram();
        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);

        GLint ok = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &ok);
        if (!ok) {
            GLint len = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
            if (len < 1)
                len = 1;
            temp_t t = temp_begin(scratch);
            char *log = push_array(scratch, char, (u64)len);
            glGetProgramInfoLog(program, len, NULL, log);
            LOG_ERROR("shader link failed: %s", log);
            temp_end(t);
            glDeleteProgram(program);
            program = 0;
        } else {
            // Linked: the stage objects are no longer needed by the program.
            glDetachShader(program, vs);
            glDetachShader(program, fs);
        }
    }

    if (vs)
        glDeleteShader(vs);
    if (fs)
        glDeleteShader(fs);
    return program;
}

u32 shader_from_files(arena_t *scratch, const char *vs_path, const char *fs_path) {
    temp_t t = temp_begin(scratch);
    file_data_t vs = file_read(scratch, vs_path);
    file_data_t fs = file_read(scratch, fs_path);

    u32 program = 0;
    if (vs.data && fs.data)
        program = shader_from_src(scratch, (const char *)vs.data, (const char *)fs.data);

    temp_end(t);
    return program;
}
