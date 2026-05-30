#include "texture.h"

#include "../core/file.h"
#include "../core/log.h"
#include "gl.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

u32 texture_load(arena_t *scratch, const char *path) {
    temp_t t = temp_begin(scratch);
    file_data_t f = file_read(scratch, path);
    if (!f.data) {
        temp_end(t);
        return 0;
    }

    stbi_set_flip_vertically_on_load(1); // GL's UV origin is bottom-left
    int w, h, channels;
    stbi_uc *pixels = stbi_load_from_memory(f.data, (int)f.size, &w, &h, &channels, 4);
    temp_end(t);

    if (!pixels) {
        LOG_ERROR("texture_load: %s: %s", path, stbi_failure_reason());
        return 0;
    }

    u32 tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // KayKit palette atlases pack tiny colour swatches in a sea of grey; bilinear + mipmaps bleed
    // that grey into them (muddy/near-black meshes), so sample nearest with no mipmaps.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(pixels);
    return tex;
}
