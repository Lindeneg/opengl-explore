#ifndef TRADING_STUFF_GL_H_
#define TRADING_STUFF_GL_H_

#include <GL/glcorearb.h> // types, enums, PFN typedefs only (no prototypes)

#include "../common.h"

#define GL_FUNCTIONS(X)                                                                            \
    X(PFNGLENABLEPROC, glEnable)                                                                   \
    X(PFNGLCREATESHADERPROC, glCreateShader)                                                       \
    X(PFNGLSHADERSOURCEPROC, glShaderSource)                                                       \
    X(PFNGLCOMPILESHADERPROC, glCompileShader)                                                     \
    X(PFNGLGETSHADERIVPROC, glGetShaderiv)                                                         \
    X(PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog)                                               \
    X(PFNGLDELETESHADERPROC, glDeleteShader)                                                       \
    X(PFNGLCREATEPROGRAMPROC, glCreateProgram)                                                     \
    X(PFNGLATTACHSHADERPROC, glAttachShader)                                                       \
    X(PFNGLDETACHSHADERPROC, glDetachShader)                                                       \
    X(PFNGLLINKPROGRAMPROC, glLinkProgram)                                                         \
    X(PFNGLGETPROGRAMIVPROC, glGetProgramiv)                                                       \
    X(PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog)                                             \
    X(PFNGLUSEPROGRAMPROC, glUseProgram)                                                           \
    X(PFNGLDELETEPROGRAMPROC, glDeleteProgram)                                                     \
    X(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays)                                                 \
    X(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray)                                                 \
    X(PFNGLDELETEVERTEXARRAYSPROC, glDeleteVertexArrays)                                           \
    X(PFNGLGENBUFFERSPROC, glGenBuffers)                                                           \
    X(PFNGLBINDBUFFERPROC, glBindBuffer)                                                           \
    X(PFNGLBUFFERDATAPROC, glBufferData)                                                           \
    X(PFNGLDELETEBUFFERSPROC, glDeleteBuffers)                                                     \
    X(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer)                                         \
    X(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray)                                 \
    X(PFNGLCLEARCOLORPROC, glClearColor)                                                           \
    X(PFNGLCLEARPROC, glClear)                                                                     \
    X(PFNGLVIEWPORTPROC, glViewport)                                                               \
    X(PFNGLDRAWARRAYSPROC, glDrawArrays)                                                           \
    X(PFNGLDEBUGMESSAGECALLBACKPROC, glDebugMessageCallback)                                       \
    X(PFNGLDEBUGMESSAGECONTROLPROC, glDebugMessageControl)                                         \
    X(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation)                                           \
    X(PFNGLUNIFORMMATRIX4FVPROC, glUniformMatrix4fv)                                               \
    X(PFNGLDRAWELEMENTSPROC, glDrawElements)

#define X(type, name) extern type name;
GL_FUNCTIONS(X)
#undef X

b32 gl_load(void_fn (*get)(const char *name));

void gl_enable_debug_output(void);

#endif // TRADING_STUFF_GL_H_
