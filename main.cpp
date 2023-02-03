/**
 * @file main.cpp
 * @author Carlos Salguero
 * @brief Main file of the program
 * @version 0.1
 * @date 2023-02-02
 *
 * @copyright Copyright (c) 2023
 *
 */

// C++ Standard Library
#include <cstdio>
#include <cstdint>
#include <limits>

// OpenGl Headers & Libraries
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Errors
#include "OpenGL/Error/glError.h"

// Project Headers
#include "OpenGL/Window/Window.cpp"
#include "OpenGL/Buffer/Buffer.cpp"

// Structs
#include "Structs.h"

// Global Variables
bool game_running{false};
bool fire_pressed{false};
int move_direction{0};

// Function Prototypes
// Shader & Program Validation
void validate_shader(GLuint, const char *file = 0);
bool validate_program(GLuint);

// Callbacks
void error_callback(int, const char *);
void key_callback(GLFWwindow *, int, int, int, int);

// Buffer
void buffer_clear(Buffer &, uint32_t);
void buffer_draw_sprite(
    Buffer *, const Sprite &, size_t, size_t, uint32_t);
void buffer_draw_number(
    Buffer *, const Sprite &, size_t, size_t, uint32_t);
void buffer_draw_text(
    Buffer *, const Sprite &, const char *, size_t, size_t, uint32_t);

// Game Functions
const char *create_fragment_shader();
const char *create_vertex_shader();

uint32_t xorshift32(uint32_t *);
double random(uint32_t *);
bool sprite_overlap_check(const Sprite &, size_t, size_t,
                          const Sprite &, size_t, size_t);
uint32_t rgb_to_uint32(uint8_t, uint8_t, uint8_t);

// Game Utilities
void generate_alien_sprites(Sprite *);
void generate_alien_death_sprites(Sprite *);
void generate_player_sprite(Sprite *);
void generate_spritesheet(Sprite *);
void generate_player_bullet_sprite(Sprite *);
void generate_alien_bullet_sprite(Sprite *);

// Main Function
int main(int argc, char **argv)
{
    glfwSetErrorCallback(error_callback);

    // Window
    const size_t buffer_width{224};
    const size_t buffer_height{256};

    Window window{buffer_width, buffer_height, "Space Invaders"};
    glfwSetKeyCallback(window.get_window(), key_callback);

    gl_debug(__FILE__, __LINE__);

    // Vsync
    glfwSwapInterval(1);
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

    // Graphics Buffer
    Buffer buffer{buffer_width, buffer_height};
    buffer_clear(buffer, 0);

    // Texture & Presenting Buffer
    GLuint buffer_texture;
    glGenTextures(1, &buffer_texture);
    glBindTexture(GL_TEXTURE_2D, buffer_texture);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB8,
        buffer.get_width(), buffer.get_height(),
        0,
        GL_RGBA,
        GL_UNSIGNED_INT_8_8_8_8,
        buffer.get_data());

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        GL_NEAREST);
    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MAG_FILTER,
        GL_NEAREST);

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_S,
        GL_CLAMP_TO_EDGE);

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_T,
        GL_CLAMP_TO_EDGE);

    // Vao
    GLuint vao;
    glGenVertexArrays(1, &vao);

    // Creating the shaders for display buffer
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
        fprintf(stderr,
                "Error while linking shader program.\n");
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

    // Generating the sprites
    Sprite alien_sprites[6];
    generate_alien_sprites(alien_sprites);

    Sprite alien_death_sprite;
    generate_alien_death_sprites(&alien_death_sprite);

    Sprite player_sprite;
    generate_player_sprite(&player_sprite);

    Sprite spritesheet;
    generate_spritesheet(&spritesheet);

    Sprite number_spritesheet = spritesheet;
    number_spritesheet.data += 16 * 35;

    Sprite player_bullet_sprite;
    generate_player_bullet_sprite(&player_bullet_sprite);

    Sprite alien_bullet_sprite[2];
    generate_alien_bullet_sprite(alien_bullet_sprite);

    SpriteAnimation alien_bullet_animation;
    alien_bullet_animation.loop = true;
    alien_bullet_animation.num_frames = 2;
    alien_bullet_animation.frame_duration = 5;
    alien_bullet_animation.time = 0;

    alien_bullet_animation.frames = new Sprite *[2];
    alien_bullet_animation.frames[0] = &alien_bullet_sprite[0];
    alien_bullet_animation.frames[1] = &alien_bullet_sprite[1];

    SpriteAnimation alien_animation[3];
    size_t update_frequency{120};

    for (size_t i{}; i < 3; ++i)
    {
        alien_animation[i].loop = true;
        alien_animation[i].num_frames = 2;
        alien_animation[i].frame_duration = update_frequency;
        alien_animation[i].time = 0;

        alien_animation[i].frames = new Sprite *[2];
        alien_animation[i].frames[0] = &alien_sprites[i * 2];
        alien_animation[i].frames[1] = &alien_sprites[i * 2 + 1];
    }

    Game game;

    game.width = buffer_width;
    game.height = buffer_height;
    game.num_bullets = 0;
    game.num_aliens = 55;
    game.alienș = new Alien[game.num_aliens];

    game.player->x = 112 - 5;
    game.player->y = 32;
    game.player->lives = 3;

    size_t alien_swarm_pos{24};
    size_t alien_swarm_max_pos{game.width - 16 * 11 - 3};
    size_t aliens_killed{0};
    size_t alien_update_timer{0};

    bool should_change_speed{false};

    for (size_t xi{}; xi < 11; ++xi)
    {
        for (size_t yi{}; yi < 5; ++yi)
        {
            Alien &alien = game.alienș[xi * 5 + yi];
            alien.type = (5 - yi) / 2 + 1;

            const Sprite &sprite = alien_sprites[2 * (alien.type - 1)];

            alien.x = 16 * xi + alien_swarm_pos +
                      (alien_death_sprite.width - sprite.width) / 2;
            alien.y = 17 * yi + 128;
        }
    }

    uint8_t *death_counters = new uint8_t[game.num_aliens];

    for (size_t i{}; i < game.num_aliens; ++i)
        death_counters[i] = 10;

    uint32_t clear_color = rgb_to_uint32(0, 0, 0);
    uint32_t rng = 13;

    int alien_dir{4};
    size_t score{0};
    size_t credits{0};

    game_running = true;

    // Game Loop
    while (!glfwWindowShouldClose(window.get_window()) && game_running)
    {
    }

    // Garbage Collection
    glDeleteVertexArrays(1, &vao);
    glfwDestroyWindow(window.get_window());
    glfwTerminate();
}

// Function Implementations
// Shader & Program Validation
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

// Game Callbacks
/**
 * @brief
 * Error callback function
 * @param error Error code
 * @param description Error description
 */
void error_callback(int error, const char *description)
{
    fprintf(stderr, "Error: %s\n", description);
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
 * Clears the graphics buffer.
 * @param buffer Buffer to clear
 * @param color Color to clear the buffer with
 */
void buffer_clear(Buffer &buffer, uint32_t color)
{
    uint32_t *pixel{buffer.get_data()};

    for (size_t i{}; i < buffer.get_width() * buffer.get_height(); ++i)
        *pixel++ = color;
}

/**
 * @brief
 * Draws a sprite to the buffer
 * @param buffer Buffer to draw to
 * @param sprite Sprite to draw
 * @param x X position to draw the sprite at
 * @param y Y position to draw the sprite at
 * @param color Color to draw the sprite with
 */
void buffer_draw_sprite(
    Buffer *buffer, const Sprite &sprite, size_t x, size_t y, uint32_t color)
{
    uint32_t *pixel{buffer->get_data() + y * buffer->get_width() + x};

    for (size_t yi{}; yi < sprite.height; ++yi)
    {
        for (size_t xi{}; xi < sprite.width; ++xi)
        {
            if (sprite.data[yi * sprite.width + xi] != 0)
                pixel[xi] = color;
        }

        pixel += buffer->get_width();
    }
}

/**
 * @brief
 * Draws a number to the buffer
 * @param buffer Buffer to draw to
 * @param sprite Number to draw
 * @param number Number to draw
 * @param x X position to draw the sprite at
 * @param y Y position to draw the sprite at
 * @param color Color to draw the sprite with
 */
void buffer_draw_number(
    Buffer *buffer, const Sprite &sprite, size_t number,
    size_t x, size_t y, uint32_t color)
{
    uint8_t digits[64];
    size_t digit_count{0};

    size_t num{number};

    do
    {
        digits[digit_count++] = num % 10;
        num /= 10;
    } while (num > 0);

    size_t xp = x;
    size_t stride = sprite.width * sprite.height;
    Sprite digit_sprite = sprite;

    for (size_t i{}; i < digit_count; ++i)
    {
        uint8_t digit = digits[digit_count - i - 1];
        digit_sprite.data = sprite.data + digit * stride;

        buffer_draw_sprite(buffer, digit_sprite, xp, y, color);
        xp += sprite.width;
    }
}

/**
 * @brief
 * Draws a string to the buffer
 * @param buffer Buffer to draw to
 * @param sprite String to draw
 * @param string String to draw
 * @param x X position to draw the sprite at
 * @param y Y position to draw the sprite at
 * @param color Color to draw the sprite with
 */
void buffer_draw_string(
    Buffer *buffer, const Sprite &sprite, const char *string,
    size_t x, size_t y, uint32_t color)
{
    size_t xp = x;
    size_t stride = sprite.width * sprite.height;
    Sprite char_sprite = sprite;

    for (size_t i{}; string[i] != '\0'; ++i)
    {
        char c = string[i];
        char_sprite.data = sprite.data + (c - 32) * stride;

        buffer_draw_sprite(buffer, char_sprite, xp, y, color);
        xp += sprite.width;
    }
}

// Game Functions
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

/**
 * @brief
 * Generates a random number using the xorshift algorithm
 * @param state  State of the random number generator
 * @return uint32_t Random number
 * @see https://en.wikipedia.org/wiki/Xorshift
 */
uint32_t xorshift(uint32_t *state)
{
    uint32_t x = *state;

    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;

    return x;
}

/**
 * @brief
 * Generates a random number between 0 and 1 using the xorshift algorithm
 * @param state State of the random number generator
 * @return double Random number
 */
double random(uint32_t *state)
{
    return (double)xorshift(state) / (double)UINT32_MAX;
}

/**
 * @brief
 * Checks for Sprite collisions
 * @param a First sprite
 * @param x_a X position of the first sprite
 * @param y_a Y position of the first sprite
 * @param b Second sprite
 * @param x_b X position of the second sprite
 * @param y_b Y position of the second sprite
 */
bool sprite_collision(
    const Sprite &a, size_t x_a, size_t y_a,
    const Sprite &b, size_t x_b, size_t y_b)
{
    if (x_a < x_b + b.width && x_a + a.width > x_b &&
        y_a < y_b + b.height && y_a + a.height > y_b)
        return true;

    return false;
}

uint32_t rgb_to_uint32(uint8_t r, uint8_t g, uint8_t b)
{
    return (r << 24) | (g << 16) | (b << 8) | 0xFF;
}

// Game Utilities
/**
 * @brief
 * Generates the alien sprites
 * @param alien_sprites
 */
void generate_alien_sprites(Sprite *alien_sprites)
{
    alien_sprites[0].width = 8;
    alien_sprites[0].height = 8;
    alien_sprites[0].data = new uint8_t[64]{
        0, 0, 0, 1, 1, 0, 0, 0, // ...@@...
        0, 0, 1, 1, 1, 1, 0, 0, // ..@@@@..
        0, 1, 1, 1, 1, 1, 1, 0, // .@@@@@@.
        1, 1, 0, 1, 1, 0, 1, 1, // @@.@@.@@
        1, 1, 1, 1, 1, 1, 1, 1, // @@@@@@@@
        0, 1, 0, 1, 1, 0, 1, 0, // .@.@@.@.
        1, 0, 0, 0, 0, 0, 0, 1, // @......@
        0, 1, 0, 0, 0, 0, 1, 0  // .@....@.
    };

    alien_sprites[1].width = 8;
    alien_sprites[1].height = 8;
    alien_sprites[1].data = new uint8_t[64]{
        0, 0, 0, 1, 1, 0, 0, 0, // ...@@...
        0, 0, 1, 1, 1, 1, 0, 0, // ..@@@@..
        0, 1, 1, 1, 1, 1, 1, 0, // .@@@@@@.
        1, 1, 0, 1, 1, 0, 1, 1, // @@.@@.@@
        1, 1, 1, 1, 1, 1, 1, 1, // @@@@@@@@
        0, 0, 1, 0, 0, 1, 0, 0, // ..@..@..
        0, 1, 0, 1, 1, 0, 1, 0, // .@.@@.@.
        1, 0, 1, 0, 0, 1, 0, 1  // @.@..@.@
    };

    alien_sprites[2].width = 11;
    alien_sprites[2].height = 8;
    alien_sprites[2].data = new uint8_t[88]{
        0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, // ..@.....@..
        0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, // ...@...@...
        0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, // ..@@@@@@@..
        0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, // .@@.@@@.@@.
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // @@@@@@@@@@@
        1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, // @.@@@@@@@.@
        1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, // @.@.....@.@
        0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0  // ...@@.@@...
    };

    alien_sprites[3].width = 11;
    alien_sprites[3].height = 8;
    alien_sprites[3].data = new uint8_t[88]{
        0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, // ..@.....@..
        1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, // @..@...@..@
        1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, // @.@@@@@@@.@
        1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, // @@@.@@@.@@@
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // @@@@@@@@@@@
        0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, // .@@@@@@@@@.
        0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, // ..@.....@..
        0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0  // .@.......@.
    };

    alien_sprites[4].width = 12;
    alien_sprites[4].height = 8;
    alien_sprites[4].data = new uint8_t[96]{
        0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, // ....@@@@....
        0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, // .@@@@@@@@@@.
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // @@@@@@@@@@@@
        1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, // @@@..@@..@@@
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // @@@@@@@@@@@@
        0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, // ...@@..@@...
        0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, // ..@@.@@.@@..
        1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1  // @@........@@
    };

    alien_sprites[5].width = 12;
    alien_sprites[5].height = 8;
    alien_sprites[5].data = new uint8_t[96]{
        0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, // ....@@@@....
        0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, // .@@@@@@@@@@.
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // @@@@@@@@@@@@
        1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, // @@@..@@..@@@
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // @@@@@@@@@@@@
        0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, // ..@@@..@@@..
        0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, // .@@..@@..@@.
        0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0  // ..@@....@@..
    };
}

/**
 * @brief
 * Generate the sprite for the alien death animation
 * @param alien_death The sprite to generate
 */
void generate_alien_death_sprites(Sprite *alien_death)
{
    alien_death->width = 13;
    alien_death->height = 7;
    alien_death->data = new uint8_t[91]{
        0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, // .@..@...@..@.
        0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, // ..@..@.@..@..
        0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, // ...@.....@...
        1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, // @@.........@@
        0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, // ...@.....@...
        0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, // ..@..@.@..@..
        0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0  // .@..@...@..@.
    };
}

/**
 * @brief
 * Generate the player sprite
 * @param player The sprite to generate
 */
void generate_player_sprite(Sprite *player)
{
    player->width = 11;
    player->height = 7;
    player->data = new uint8_t[77]{
        0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, // .....@.....
        0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, // ....@@@....
        0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, // ....@@@....
        0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, // .@@@@@@@@@.
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // @@@@@@@@@@@
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // @@@@@@@@@@@
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // @@@@@@@@@@@
    };
}

/**
 * @brief
 * Generate the sprite sheet
 * @param spritesheet The sprite to generate
 */
void generate_spritesheet(Sprite *spritesheet)
{
    spritesheet->width = 5;
    spritesheet->height = 7;

    spritesheet->data = new uint8_t[65 * 35]{
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
        0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0,
        0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0,
        1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1,
        0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1,
        0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0,
        0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
        0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0,

        0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0,
        0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0,
        0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0,
        0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0,
        1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0,
        0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0,
        1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0,
        0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0,
        0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0,

        0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0,
        0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0,
        0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
        0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0,

        0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1,
        1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0,
        0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0,
        1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0,
        1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0,
        0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0,
        1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1,
        0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0,
        0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0,
        1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1,
        1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1,
        1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1,
        0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0,
        1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0,
        0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1,
        1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1,
        0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0,
        1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0,
        1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0,
        1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0,
        1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1,
        1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1,
        1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0,
        1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1,

        0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1,
        0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0,
        1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0,
        0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
        0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
}

/**
 * @brief
 * Generate player bullet sprite
 * @param player_bullet The sprite to be generated
 */
void generate_player_bullet_sprite(Sprite *player_bullet)
{
    player_bullet->width = 3;
    player_bullet->height = 3;
    player_bullet->data = new uint8_t[3]{1, 1, 1};
}

/**
 * @brief
 * Generate enemy bullet sprite
 * @param alien_bullet The sprite to be generated
 */
void generate_alien_bullet_sprite(Sprite *alien_bullet)
{
    alien_bullet[0].width = 3;
    alien_bullet[0].height = 7;
    alien_bullet[0].data = new uint8_t[21]{
        0,
        1,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        1,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
    };

    alien_bullet[1].width = 3;
    alien_bullet[1].height = 7;
    alien_bullet[1].data = new uint8_t[21]{
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        1,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        1,
        0,
    };
}