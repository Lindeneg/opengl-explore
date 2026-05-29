#ifndef TRADING_STUFF_GL_H_
#define TRADING_STUFF_GL_H_

#include "../common.h"

#include <GL/glcorearb.h> // types, enums, PFN typedefs only (no prototypes)

// Hand-loaded OpenGL core bindings. We declare a function pointer for every GL
// entry point we use (the PFN...PROC typedefs come from glcorearb.h) and load
// them at runtime via gl_load(). No generator, no GLEW.

#define GL_FUNCTIONS(X)                                            \
    X(PFNGLENABLEPROC, glEnable)                                   \
    X(PFNGLCREATESHADERPROC, glCreateShader)                       \
    X(PFNGLSHADERSOURCEPROC, glShaderSource)                       \
    X(PFNGLCOMPILESHADERPROC, glCompileShader)                     \
    X(PFNGLGETSHADERIVPROC, glGetShaderiv)                         \
    X(PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog)               \
    X(PFNGLDELETESHADERPROC, glDeleteShader)                       \
    X(PFNGLCREATEPROGRAMPROC, glCreateProgram)                     \
    X(PFNGLATTACHSHADERPROC, glAttachShader)                       \
    X(PFNGLDETACHSHADERPROC, glDetachShader)                       \
    X(PFNGLLINKPROGRAMPROC, glLinkProgram)                         \
    X(PFNGLGETPROGRAMIVPROC, glGetProgramiv)                       \
    X(PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog)             \
    X(PFNGLUSEPROGRAMPROC, glUseProgram)                           \
    X(PFNGLDELETEPROGRAMPROC, glDeleteProgram)                     \
    X(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays)                 \
    X(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray)                 \
    X(PFNGLDELETEVERTEXARRAYSPROC, glDeleteVertexArrays)           \
    X(PFNGLGENBUFFERSPROC, glGenBuffers)                           \
    X(PFNGLBINDBUFFERPROC, glBindBuffer)                           \
    X(PFNGLBUFFERDATAPROC, glBufferData)                           \
    X(PFNGLDELETEBUFFERSPROC, glDeleteBuffers)                     \
    X(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer)         \
    X(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray) \
    X(PFNGLCLEARCOLORPROC, glClearColor)                           \
    X(PFNGLCLEARPROC, glClear)                                     \
    X(PFNGLVIEWPORTPROC, glViewport)                               \
    X(PFNGLDRAWARRAYSPROC, glDrawArrays)                           \
    X(PFNGLDEBUGMESSAGECALLBACKPROC, glDebugMessageCallback)       \
    X(PFNGLDEBUGMESSAGECONTROLPROC, glDebugMessageControl)

#define X(type, name) extern type name;
GL_FUNCTIONS(X)
#undef X

// Load all GL function pointers via `get` (the window layer wraps
// glfwGetProcAddress). Returns false if any entry point is missing.
b32 gl_load(void_fn (*get)(const char *name));

// Install a debug-output callback (GL 4.3) that routes GL messages to the logger.
void gl_enable_debug_output(void);

#endif // TRADING_STUFF_GL_H_
