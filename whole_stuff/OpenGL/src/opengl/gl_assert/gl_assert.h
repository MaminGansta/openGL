#pragma once

#define GLEW_STATIC
#include "GL/glew.h"

// error handler
#if _DEBUG
#define ASSERT(x) if (!(x)) __debugbreak();
#define glcall(func) GLClearError();\
        func;\
        ASSERT(GLLogCall(#func, __FILE__, __LINE__))

static void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}
static bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        printf("[openGL Error] > %d \n %s : %d \n" , error, file, line);
        return false;
    }
    return true;
}
#else
#define glcall(func) (func);
#endif
