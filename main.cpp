/**
 * @file main.cpp
 * @author Carlos Salguero
 * @brief Main file for the game
 * @version 0.1
 * @date 2022-12-15
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <cstdio>
#include <cstdint>
#include <limits>

// OpenGL Headers
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// OpenGL Error
#include "OpenGL/Error/glError.h"

/* ---- Custom Headers ---- */
#include "OpenGL/Window/Window.cpp"

/* ---- Function Prototypes ---- */
void error_callback(int, const char *);
void key_callback(GLFWwindow *, int, int, int, int);

/* ---- Global variables ---- */
bool game_running{true};
bool fire_pressed{false};

int move_direction{};

/* ---- Main Function ---- */
int main(int argc, char **argv)
{
    glfwSetErrorCallback(error_callback);

    // Window creation
    const size_t buffer_width{224};
    const size_t buffer_height{256};

    Window window(buffer_width, buffer_height, "Space Invaders");
    glfwSetKeyCallback(window.getWindow(), key_callback);

    gl_debug(__FILE__, __LINE__);

    // VSync
    glfwSwapInterval(1);

    glClearColor(1.0f, 0.0, 0.0f, 1.0f);

    // Main loop
    while (game_running)
    {
        window.update();

        window.clear();
    }

    window.terminate();
}

/* ---- Function Implementation ---- */
/**
 * @brief
 * Error callback function
 * @param error Error code
 * @param description Error description
 * @return void
 */
void error_callback(int error, const char *description)
{
    fprintf(stderr, "Error: %s (%d)", description);
}

/**
 * @brief
 * Key callback function
 * @param window Window pointer
 * @param key Key code
 * @param scancode Scancode
 * @param action Action code
 * @param mods Modifiers
 * @return void
 */
void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods)
{

    switch (key)
    {
    case GLFW_KEY_ESCAPE:
        if (action == GLFW_PRESS)
            game_running = false;

        break;

    case GLFW_KEY_A:
        if (action == GLFW_PRESS)
            move_direction += 1;

        else if (action == GLFW_RELEASE)
            move_direction -= 1;

        break;

    case GLFW_KEY_D:
        if (action == GLFW_PRESS)
            move_direction -= 1;

        else if (action == GLFW_RELEASE)
            move_direction += 1;

        break;

    case GLFW_KEY_SPACE:
        if (action == GLFW_PRESS)
            fire_pressed = true;

        else if (action == GLFW_RELEASE)
            fire_pressed = false;

        break;

    default:
        break;
    }
}