#include "gl.h"

#include "../core/log.h"

#define X(type, name) type name = NULL;
GL_FUNCTIONS(X)
#undef X

b32 gl_load(void_fn (*get)(const char *name)) {
    b32 ok = true;
#define X(type, name)                                                                              \
    name = (type)get(#name);                                                                       \
    if (!name) {                                                                                   \
        LOG_ERROR("GL: failed to load %s", #name);                                                 \
        ok = false;                                                                                \
    }
    GL_FUNCTIONS(X)
#undef X
    return ok;
}

static void APIENTRY gl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
                                       GLsizei length, const GLchar *message, const void *user) {
    (void)source;
    (void)type;
    (void)id;
    (void)length;
    (void)user;

    if (severity == GL_DEBUG_SEVERITY_HIGH)
        LOG_ERROR("GL: %s", message);
    else if (severity == GL_DEBUG_SEVERITY_MEDIUM || severity == GL_DEBUG_SEVERITY_LOW)
        LOG_WARN("GL: %s", message);
    else // GL_DEBUG_SEVERITY_NOTIFICATION
        LOG_TRACE("GL: %s", message);
}

void gl_enable_debug_output(void) {
    if (!glDebugMessageCallback) {
        LOG_WARN("GL: debug output unavailable");
        return;
    }
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // callback fires on the offending call
    glDebugMessageCallback(gl_debug_callback, NULL);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
}
