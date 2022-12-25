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
#include "Sprite/Sprite.cpp"

// Player, Bullet, Alien
#include "Player/Player.cpp"
#include "Bullet/Bullet.cpp"
#include "Alien/Alien.cpp"

/* ---- Function Prototypes ---- */
// Validation
void validate_shader(GLuint, const char *);
bool validate_program(GLuint);

// Callbacks
void error_callback(int, const char *);
void key_callback(GLFWwindow *, int, int, int, int);

// Buffer
void buffer_clear(Buffer &, uint32_t);
void buffer_draw_sprite(Buffer *, const Sprite &, size_t, size_t, uint32_t);
void buffer_draw_number(Buffer *, const Sprite &, size_t, size_t,
                        uint32_t, size_t);
void buffer_draw_text(Buffer *, const Sprite &, const char *,
                      size_t, size_t, uint32_t);

// Shifts and random
uint32_t xorshift32(uint32_t *);
double random(uint32_t *);

// Game
bool sprite_overlap_check(const Sprite &, size_t, size_t,
                          const Sprite &, size_t, size_t);

uint32_t rgb_to_uint32(uint8_t, uint8_t, uint8_t);

// Generation

/* ---- Global variables ---- */
bool game_running{false};
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
    game.set_num_bullets(0);
    game.set_num_aliens(55);

    game.get_player().set_x(112 - 5);
    game.get_player().set_y(256 - 16);
    game.get_player().set_life(3);

    size_t alien_swarm_pos{24};
    size_t max_alien_swarm_pos{game.get_width() - 16 * 11 - 3};
    size_t aliens_killed{0};
    size_t alien_update_timer{0};

    bool should_change_speed{false};

    for (size_t xi{}; xi < 11; ++xi)
    {
        for (size_t yi{}; yi < 5; ++yi)
        {
            Alien &alien = game.get_aliens()[xi * 5 + yi];
            alien.set_type((5 - yi) / 2 + 1);

            const Sprite &sprite = alien_sprites[2 * (alien.get_type() - 1)];
        }
    }

    uint8_t *death_counters = new uint8_t[game.get_num_aliens()]{};

    for (size_t i{}; i < game.get_num_aliens(); ++i)
        death_counters[i] = 10;

    uint32_t clear_color{rgb_to_uint32(0, 128, 0)};
    uint32_t rng{13};

    int alien_direction{4};
    size_t score{};
    size_t high_score{};
    size_t credits{};

    game_running = true;

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
 * Draw a sprite to the buffer
 * @param buffer Buffer to draw to
 * @param sprite Sprite to draw
 * @param x X position
 * @param y Y position
 * @param color Color to draw the sprite with
 */
void buffer_draw_sprite(Buffer *buffer, const Sprite &sprite,
                        size_t x, size_t y, uint32_t color)
{
    for (size_t xi{}; xi < sprite.get_width(); ++xi)
    {
        for (size_t yi{}; yi < sprite.get_height(); ++yi)
        {
            if (sprite.get_pixels()[yi * sprite.get_width() + xi] &&
                (sprite.get_height() - 1 + y - yi) < buffer->get_height() &&
                (x + xi) < buffer->get_width())

                buffer->get_data()[(sprite.get_height() - 1 + y - yi) *
                                       buffer->get_width() +
                                   x + xi] = color;
        }
    }
}

/**
 * @brief
 * Draw a number to the buffer
 * @param buffer Buffer to draw to
 * @param sprite Sprite to draw
 * @param x X position
 * @param y Y position
 * @param color Color to draw the sprite with
 * @param number Number to draw
 */
void buffer_draw_number(Buffer *buffer, const Sprite &sprite, size_t x,
                        size_t y, uint32_t color, size_t number)
{
    uint8_t digits[64]{};
    size_t num_digits{};
    size_t current_number = number;

    while (current_number > 0)
    {
        digits[num_digits++] = current_number % 10;
        current_number /= 10;
    }

    size_t xp = x;
    size_t stride = sprite.get_width() * sprite.get_height();
    Sprite numbder_spritesheet = sprite;

    for (size_t i{}; i < num_digits; ++i)
    {
        uint8_t digit = digits[num_digits - i - 1];
        numbder_spritesheet.set_pixels(sprite.get_pixels() + digit * stride);

        buffer_draw_sprite(buffer, numbder_spritesheet, xp, y, color);
        xp += sprite.get_width() + 1;
    }
}

/**
 * @brief
 * Draws a string to the buffer
 * @param buffer Buffer to draw to
 * @param sprite Sprite to draw
 * @param text Text to draw
 * @param x X position
 * @param y Y position
 * @param color Color to draw the sprite with
 */
void buffer_draw_text(Buffer *buffer, const Sprite &sprite,
                      const char *text, size_t x, size_t y, uint32_t color)
{
    size_t xp = x;
    size_t stride = sprite.get_width() * sprite.get_height();
    Sprite letter_spritesheet = sprite;

    for (const char *c{text}; *c != '\0'; ++c)
    {
        uint8_t letter = *c - 32;
        letter_spritesheet.set_pixels(sprite.get_pixels() + letter * stride);

        buffer_draw_sprite(buffer, letter_spritesheet, xp, y, color);
        xp += sprite.get_width() + 1;
    }
}

// Shifts and random
/**
 * @brief
 * Xorshift32 random number generator
 * @see https://en.wikipedia.org/wiki/Xorshift
 * @param state State of the generator
 * @return uint32_t
 */
uint32_t xorshift32(uint32_t &state)
{
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return state;
}

double random(uint32_t &state)
{
    return static_cast<double>(xorshift32(state)) / UINT32_MAX;
}

// Game
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