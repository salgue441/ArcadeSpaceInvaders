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

/* ---- Custom Headers ---- */
// OpenGL Error & Window
#include "OpenGL/Error/glError.h"
#include "OpenGL/Window/Window.cpp"

// Buffer
#include "OpenGL/Buffer/Buffer.cpp"

// Game
#include "Game/Game.cpp"

// Sprites

// Player
#include "Player/Player.cpp"

/* ---- Function Prototypes ---- */
// Callbacks
void error_callback(int, const char *);
void key_callback(GLFWwindow *, int, int, int, int);

// Buffer
void buffer_clear(Buffer &, uint32_t);
uint32_t rgb_to_uint32(uint8_t, uint8_t, uint8_t);

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
    glfwSetKeyCallback(window.get_window(), key_callback);

    gl_debug(__FILE__, __LINE__);

    // VSync
    glfwSwapInterval(1);
    glClearColor(1.0f, 0.0, 0.0f, 1.0f);

    // Buffer creation
    Buffer buffer(buffer_width, buffer_height);
    buffer_clear(buffer, 0x00000000);

    // Game variables
    Game game(buffer_width, buffer_height);

    uint32_t clear_color{rgb_to_uint32(0, 128, 0)};

    // Main loop
    while (!glfwWindowShouldClose(window.get_window()) && game_running)
    {
        buffer_clear(buffer, clear_color);

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

// Buffer
/**
 * @brief
 * Clear the buffer
 * @param buffer Buffer to clear
 * @param color Color to clear the buffer with
 * @return void
 */
void buffer_clear(Buffer &buffer, uint32_t color)
{
    uint32_t *pixel{buffer.get_data()};

    for (size_t i{0}; i < buffer.get_width() * buffer.get_height(); i++)
        pixel[i] = color;
}

/**
 * @brief
 * Convert RGB values to a uint32_t
 * @param r Red value
 * @param g Green value
 * @param b Blue value
 * @return uint32_t
 */
uint32_t rgb_to_uint32(uint8_t r, uint8_t g, uint8_t b)
{
    return (r << 24) | (g << 16) | (b << 8) | 255;
}