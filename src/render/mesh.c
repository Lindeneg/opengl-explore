#include "mesh.h"

#include "gl.h"

#define CGLTF_IMPLEMENTATION
#include <cgltf.h>

mesh_t mesh_load_gltf(arena_t *scratch, const char *path) {
    cgltf_options options = {0};
    cgltf_data *data = NULL;
    ASSERT_MSG(cgltf_parse_file(&options, path, &data) == cgltf_result_success, "gltf parse failed: %s",
               path);
    ASSERT_MSG(cgltf_load_buffers(&options, data, path) == cgltf_result_success,
               "gltf buffers failed: %s", path);

    u64 vert_count = 0;
    u64 index_count = 0;
    for (cgltf_size mi = 0; mi < data->meshes_count; ++mi) {
        cgltf_mesh *mesh = &data->meshes[mi];
        for (cgltf_size pi = 0; pi < mesh->primitives_count; ++pi) {
            cgltf_primitive *prim = &mesh->primitives[pi];
            cgltf_accessor *pos = NULL;
            for (cgltf_size ai = 0; ai < prim->attributes_count; ++ai)
                if (prim->attributes[ai].type == cgltf_attribute_type_position)
                    pos = prim->attributes[ai].data;
            if (!pos)
                continue;
            vert_count += pos->count;
            index_count += prim->indices ? prim->indices->count : pos->count;
        }
    }
    ASSERT_MSG(vert_count > 0, "gltf has no vertices: %s", path);

    temp_t t = temp_begin(scratch);
    f32 *verts = push_array(scratch, f32, vert_count * 5);
    u32 *indices = push_array(scratch, u32, index_count);

    u64 vbase = 0;
    u64 ioff = 0;
    for (cgltf_size mi = 0; mi < data->meshes_count; ++mi) {
        cgltf_mesh *mesh = &data->meshes[mi];
        for (cgltf_size pi = 0; pi < mesh->primitives_count; ++pi) {
            cgltf_primitive *prim = &mesh->primitives[pi];
            cgltf_accessor *pos = NULL;
            cgltf_accessor *uv = NULL;
            for (cgltf_size ai = 0; ai < prim->attributes_count; ++ai) {
                cgltf_attribute *a = &prim->attributes[ai];
                if (a->type == cgltf_attribute_type_position)
                    pos = a->data;
                else if (a->type == cgltf_attribute_type_texcoord)
                    uv = a->data;
            }
            if (!pos)
                continue;

            for (cgltf_size v = 0; v < pos->count; ++v) {
                f32 *out = &verts[(vbase + v) * 5];
                cgltf_accessor_read_float(pos, v, out, 3);
                if (uv) {
                    cgltf_accessor_read_float(uv, v, out + 3, 2);
                } else {
                    out[3] = 0.0f;
                    out[4] = 0.0f;
                }
            }

            if (prim->indices) {
                for (cgltf_size i = 0; i < prim->indices->count; ++i)
                    indices[ioff++] = (u32)(vbase + cgltf_accessor_read_index(prim->indices, i));
            } else {
                for (cgltf_size i = 0; i < pos->count; ++i)
                    indices[ioff++] = (u32)(vbase + i);
            }
            vbase += pos->count;
        }
    }

    cgltf_free(data);

    mesh_t m = {0};
    glGenVertexArrays(1, &m.vao);
    glBindVertexArray(m.vao);

    glGenBuffers(1, &m.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(vert_count * 5 * sizeof(f32)), verts, GL_STATIC_DRAW);

    glGenBuffers(1, &m.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)(index_count * sizeof(u32)), indices,
                 GL_STATIC_DRAW);
    m.index_count = (u32)index_count;

    GLsizei stride = (GLsizei)(5 * sizeof(f32));
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (const void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (const void *)(3 * sizeof(f32)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    temp_end(t);
    return m;
}

void mesh_destroy(mesh_t *m) {
    glDeleteBuffers(1, &m->vbo);
    glDeleteBuffers(1, &m->ebo);
    glDeleteVertexArrays(1, &m->vao);
    *m = (mesh_t){0};
}
