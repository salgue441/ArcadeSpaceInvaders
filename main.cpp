/**
 * @file main.cpp
 * @author Carlos Salguero
 * @brief Main file of the project
 * @version 0.1
 * @date 2022-12-28
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

// Game & Sprite
#include "Game/Game.cpp"
#include "Sprite/Sprite.cpp"

// Player, Bullet & Enemy
#include "Alien/Alien.cpp"

/* ---- Function prototypes ---- */
// Validation
void validate_shader(GLuint, const char *);
bool validate_program(GLuint);

// Callbacks
void error_callback(int, const char *);
void key_callback(GLFWwindow *, int, int, int, int);

// Buffer
void buffer_clear(Buffer &, uint32_t);

//   Shaders
const char *create_fragment_shader();
const char *create_vertex_shader();

// Shifts & random
uint32_t xorshift32(uint32_t *);
double random(uint32_t *);

// Game
uint32_t rgb_to_uint32(uint8_t, uint8_t, uint8_t);
void generate_alien_sprites(Sprite *);
void generate_alien_death_sprite(Sprite &);

/* ---- Global variables ---- */
bool game_running{false};
bool fire_pressed{false};
int move_direction{};

/* ---- Main function ---- */
int main(int argc, char **argv)
{
    glfwSetErrorCallback(error_callback);

    // Window creation
    const size_t buffer_width{224};
    const size_t buffer_height{256};

    Window window{buffer_width, buffer_height, "Space Invaders"};
    glfwSetKeyCallback(window.get_window(), key_callback);

    gl_debug(__FILE__, __LINE__);

    // Vsync
    glfwSwapInterval(1);
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

    // Buffer creation
    Buffer buffer(buffer_width, buffer_height);
    buffer_clear(buffer, 0x00000000);

    // Textures
    GLuint texture;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8,
                 buffer.get_width(), buffer.get_height(), 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, buffer.get_data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Vao
    GLuint vao;
    glGenVertexArrays(1, &vao);

    // Creating the shaders
    static const char *vertex_shader = create_vertex_shader();
    static const char *fragment_shader = create_fragment_shader();
    GLuint shader_id = glCreateProgram();

    {
        // Vertex shader
        GLuint vertex_vp = glCreateShader(GL_VERTEX_SHADER);

        glShaderSource(vertex_vp, 1, &vertex_shader, nullptr);
        glCompileShader(vertex_vp);

        validate_shader(shader_id, vertex_shader);
        glAttachShader(shader_id, vertex_vp);

        glDeleteShader(vertex_vp);
    }

    {
        // Fragment shader
        GLuint fragment_vp = glCreateShader(GL_FRAGMENT_SHADER);

        glShaderSource(fragment_vp, 1, &fragment_shader, nullptr);
        glCompileShader(fragment_vp);

        validate_shader(shader_id, fragment_shader);
        glAttachShader(shader_id, fragment_vp);

        glDeleteShader(fragment_vp);
    }

    glLinkProgram(shader_id);

    if (!validate_program(shader_id))
    {
        fprintf(stderr, "Error while linking shader program.\n");
        glfwTerminate();

        glDeleteVertexArrays(1, &vao);
        delete[] buffer.get_data();

        return EXIT_FAILURE;
    }

    glUseProgram(shader_id);

    GLint location = glGetUniformLocation(shader_id, "buffer");
    glUniform1i(location, 0);

    // OpenGL Setup
    glDisable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(vao);

    // Preparing the game
    Sprite alien_sprites[6];
    generate_alien_sprites(alien_sprites);

    Sprite alien_death_sprite;
    generate_alien_death_sprite(alien_death_sprite);

    // Game variables
    Game game;
    game.set_width(buffer_width);
    game.set_height(buffer_height);
    game.set_num_bullets(0);
    game.set_num_aliens(55);

    size_t alien_swarm_position{24};
    size_t alien_max_swarm_pos{game.get_width() - 16 * 11 - 3};
    size_t aliens_killed{};
    bool should_change_speed{false};

    for (size_t xi{}; xi < 11; ++xi)
    {
        for (size_t yi{}; yi < 5; ++yi)
        {
            Alien alien{game.get_alien(xi + yi * 11)};
            alien.set_type((5 - yi) / 2 + 1);

            const Sprite &sprite{alien_sprites[2 * (alien.get_type() - 1)]};

            alien.set_x(16 * xi + alien_swarm_position +
                        (alien_death_sprite.get_width() - sprite.get_width()) / 2);
            alien.set_y(17 * yi + 128);
        }
    }

    uint8_t *death_counter{new uint8_t[game.get_num_aliens()]};

    for (size_t i{}; i < game.get_num_aliens(); ++i)
        death_counter[i] = 10;

    uint32_t clear_color{rgb_to_uint32(0, 128, 0)};
    uint32_t rng{13};

    int alien_direction{4};
    size_t score{};
    size_t credits{};

    game_running = true;
    int player_direction{};

    while (!glfwWindowShouldClose(window.get_window()) && game_running)
    {
        // Clearing the buffer
        buffer_clear(buffer, clear_color);

        // Swap buffers
        glfwSwapBuffers(window.get_window());
        glfwPollEvents();
    }
}

/* ---- Function implementations ---- */
// Validation
/**
 * @brief
 * Validates a shader program
 * @param shader Shader program to validate
 * @param file File where the shader program is located
 */
void validate_shader(GLuint shader, const char *file)
{
    static const unsigned int BUFFER_SIZE{512};
    char buffer[BUFFER_SIZE];
    GLsizei length{};

    glGetShaderInfoLog(shader, BUFFER_SIZE, &length, buffer);

    if (length > 0)
        fprintf(stderr, "Shader %d (%s) compile error: %s\n", shader,
                (file ? file : ""), buffer);
}

/**
 * @brief
 * Validates a shader program.
 * @param program Shader program to validate
 * @return true if the shader program is valid
 * @return false if the shader program is not valid.
 */
bool validate_program(GLuint program)
{
    static const unsigned int BUFFER_SIZE{512};
    char buffer[BUFFER_SIZE];
    GLsizei length{};

    glValidateProgram(program);
    glGetProgramInfoLog(program, BUFFER_SIZE, &length, buffer);

    if (length > 0)
    {
        fprintf(stderr, "Program %d link error: %s\n", program, buffer);
        return false;
    }

    return true;
}

// Callbacks
/**
 * @brief
 * Error callback function
 * @param error Error code
 * @param description Error description
 */
void error_callback(int error, const char *description)
{
    fprintf(stderr, "Error: %s (%d)", description, error);
}

/**
 * @brief
 * Key callback function
 * @param window Window where the key was pressed
 * @param key Key that was pressed
 * @param scancode Scancode of the key
 * @param action Action that was performed
 * @param mods Modifier keys
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
 * Clears the buffer
 * @param buffer Buffer to clear
 * @param color Color to clear the buffer with
 */
void buffer_clear(Buffer &buffer, uint32_t color)
{
    uint32_t *pixel{buffer.get_data()};

    for (size_t i{}; i < buffer.get_width() * buffer.get_height(); ++i)
        *pixel++ = color;
}

//  Shaders
/**
 * @brief
 * Creates a fragment shader
 * @return const char* Fragment shader
 */
const char *create_fragment_shader()
{
    return R"(
        \n
        #version 330\n
        \n
        uniform sampler2D buffer;\n
        noperspective in vec2 texcoord;\n
        \n
        out vec3 outColor;\n
        \n
        void main(void){\n
            outColor = texture(buffer, TexCoord).rgb;\n
        }\n
    )";
}

/**
 * @brief
 * Creates a vertex shader
 * @return const char* Vertex shader
 */
const char *create_vertex_shader()
{
    return R"(
        \n
        #version 330\n
        \n
        noperspective out vec2 TexCoord;\n
        \n
        void main(void){\n
        \n
            TexCoord.x = (gl_VertexID == 2) ? 2.0 : 0.0;\n
            TexCoord.y = (gl_VertexID == 1) ? 2.0 : 0.0;\n
            \n
            gl_Position = vec4(TexCoord * 2.0 - 1.0, 0.0, 1.0);\n
        }\n
    )";
}

// Shifts and random
/**
 * @brief
 * Xorshift32 random number generator
 * @see https://en.wikipedia.org/wiki/Xorshift
 * @param state State of the generator
 * @return uint32_t
 */
uint32_t xorshift32(uint32_t *state)
{
    uint32_t x{*state};

    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;

    return x;
}

/**
 * @brief
 * Random number generator
 * @param state State of the generator
 * @return double Number generated
 */
double random(uint32_t *state)
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

// Game
/**
 * @brief
 * Modifies the alien's array to generate the sprites.
 * @param aliens Array of aliens
 */
void generate_alien_sprites(Sprite *aliens)
{
    aliens[0].set_width(8);
    aliens[0].set_height(8);
    aliens[0].set_pixels(new uint8_t[64]{
        0, 0, 0, 1, 1, 0, 0, 0,
        0, 0, 1, 1, 1, 1, 0, 0,
        0, 1, 1, 1, 1, 1, 1, 0,
        1, 1, 0, 1, 1, 0, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        0, 1, 0, 1, 1, 0, 1, 0,
        1, 0, 0, 0, 0, 0, 0, 1,
        0, 1, 0, 0, 0, 0, 1, 0});

    aliens[1].set_width(8);
    aliens[1].set_height(8);
    aliens[1].set_pixels(new uint8_t[64]{
        0, 0, 0, 1, 1, 0, 0, 0,
        0, 0, 1, 1, 1, 1, 0, 0,
        0, 1, 1, 1, 1, 1, 1, 0,
        1, 1, 0, 1, 1, 0, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        0, 0, 1, 0, 0, 1, 0, 0,
        0, 1, 0, 1, 1, 0, 1, 0,
        1, 0, 1, 0, 0, 1, 0, 1});

    aliens[2].set_width(11);
    aliens[2].set_height(8);
    aliens[2].set_pixels(new uint8_t[88]{
        0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0,
        0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0,
        0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0,
        0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1,
        1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1,
        0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0});

    aliens[3].set_width(11);
    aliens[3].set_height(8);
    aliens[3].set_pixels(new uint8_t[88]{
        0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0,
        1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1,
        1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1,
        1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
        0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0});

    aliens[4].set_width(12);
    aliens[4].set_height(8);
    aliens[4].set_pixels(new uint8_t[96]{
        0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0,
        0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0,
        0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0,
        1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1});

    aliens[5].set_width(12);
    aliens[5].set_height(8);
    aliens[5].set_pixels(new uint8_t[96]{
        0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0,
        0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0,
        0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0,
        0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0});
}

/**
 * @brief
 * Generates the alien death sprites
 * @param alien Alien sprite
 */
void generate_alien_death_sprites(Sprite &alien)
{
    alien.set_width(13);
    alien.set_height(7);

    alien.set_pixels(new uint8_t[91]{
        0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0,
        0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0,
        0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0,
        1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
        0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0,
        0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0,
        0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0});
}