/**
 * @file glError.h
 * @author Carlos Salguero
 * @brief OpenGL error cases
 * @version 0.1
 * @date 2022-12-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef GL_ERROR_H
#define GL_ERROR_H

#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

/* Error cases for OpenGL */
#define GL_ERROR_CASE(glerror) \
    case glerror:              \
        snprintf(error, sizeof(error), "%s", #glerror)

/**
 * @brief
 * Open GL debug function
 * @param file File where the error occurred
 * @param line Line where the error occurred
 */
inline void gl_debug(const char *file, int line)
{
    GLenum err;

    while ((err = glGetError()) != GL_NO_ERROR)
    {
        char error[128];

        switch (err)
        {
            GL_ERROR_CASE(GL_INVALID_ENUM);
            break;

            GL_ERROR_CASE(GL_INVALID_VALUE);
            break;

            GL_ERROR_CASE(GL_INVALID_OPERATION);
            break;

            GL_ERROR_CASE(GL_INVALID_FRAMEBUFFER_OPERATION);
            break;

            GL_ERROR_CASE(GL_OUT_OF_MEMORY);
            break;

            GL_ERROR_CASE(GL_STACK_UNDERFLOW);
            break;

            GL_ERROR_CASE(GL_STACK_OVERFLOW);
            break;

        default:
            snprintf(error, sizeof(error),
                     "Unknown error: %d", err);
            break;
        }

        fprintf(stderr, "OpenGL Error: %s - %s:%d\n", error, file, line);
    }
}

#undef GL_ERROR_CASE

#endif // !GL_ERROR_H