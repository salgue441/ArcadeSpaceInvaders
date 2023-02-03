/**
 * @file main.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-02-02
 *
 * @copyright Copyright (c) 2023
 *
 */

// C headers
#include <cstdio>
#include <cstdint>
#include <limits>

// OpenGL headers
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Errors & Window
#include "OpenGL/Error/glError.h"
#include "OpenGL/Window/Window.cpp"

// Structures
#include "Structs.h"

// Global Variables
bool game_running = false;
int move_dir = 0;
bool fire_pressed = 0;

// Function Prototypes
// Shader & Program Validation
void validate_shader(GLuint, const char *file = 0);
bool validate_program(GLuint);

// Callbacks
void error_callback(int, const char *);
void key_callback(GLFWwindow *, int, int, int, int);

// Buffer
void buffer_clear(Buffer *, uint32_t);
void buffer_draw_sprite(
    Buffer *, const Sprite &, size_t, size_t, uint32_t);
void buffer_draw_number(
    Buffer *, const Sprite &, size_t, size_t, size_t, uint32_t);
void buffer_draw_string(
    Buffer *, const Sprite &, const char *, size_t, size_t, uint32_t);

// Game Functions
uint32_t xorshift32(uint32_t *);
double random(uint32_t *);
bool sprite_collision(const Sprite &, size_t, size_t,
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
int main(int argc, char *argv[])
{
    const size_t buffer_width = 224;
    const size_t buffer_height = 256;

    glfwSetErrorCallback(error_callback);

    Window window{buffer_width, buffer_height, "Space Invaders"};

    gl_debug(__FILE__, __LINE__);

    // Vsync
    glfwSwapInterval(1);
    glClearColor(1.0, 0.0, 0.0, 1.0);

    // Create graphics buffer
    Buffer buffer;
    buffer.width = buffer_width;
    buffer.height = buffer_height;
    buffer.data = new uint32_t[buffer.width * buffer.height];

    buffer_clear(&buffer, 0);

    // Create texture for presenting buffer to OpenGL
    GLuint buffer_texture;
    glGenTextures(1, &buffer_texture);
    glBindTexture(GL_TEXTURE_2D, buffer_texture);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB8,
        buffer.width, buffer.height,
        0,
        GL_RGBA,
        GL_UNSIGNED_INT_8_8_8_8,
        buffer.data);

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

    // Create vao for generating fullscreen triangle
    GLuint vao;
    glGenVertexArrays(1, &vao);

    // Create shader for displaying buffer
    static const char *fragment_shader =
        "\n"
        "#version 330\n"
        "\n"
        "uniform sampler2D buffer;\n"
        "noperspective in vec2 TexCoord;\n"
        "\n"
        "out vec3 outColor;\n"
        "\n"
        "void main(void){\n"
        "    outColor = texture(buffer, TexCoord).rgb;\n"
        "}\n";

    static const char *vertex_shader =
        "\n"
        "#version 330\n"
        "\n"
        "noperspective out vec2 TexCoord;\n"
        "\n"
        "void main(void){\n"
        "\n"
        "    TexCoord.x = (gl_VertexID == 2)? 2.0: 0.0;\n"
        "    TexCoord.y = (gl_VertexID == 1)? 2.0: 0.0;\n"
        "    \n"
        "    gl_Position = vec4(2.0 * TexCoord - 1.0, 0.0, 1.0);\n"
        "}\n";

    GLuint shader_id = glCreateProgram();

    {
        // Create vertex shader
        GLuint shader_vp = glCreateShader(GL_VERTEX_SHADER);

        glShaderSource(shader_vp, 1, &vertex_shader, 0);
        glCompileShader(shader_vp);

        validate_shader(shader_vp, vertex_shader);
        glAttachShader(shader_id, shader_vp);

        glDeleteShader(shader_vp);
    }

    {
        // Create fragment shader
        GLuint shader_fp = glCreateShader(GL_FRAGMENT_SHADER);

        glShaderSource(shader_fp, 1, &fragment_shader, 0);
        glCompileShader(shader_fp);

        validate_shader(shader_fp, fragment_shader);
        glAttachShader(shader_id, shader_fp);

        glDeleteShader(shader_fp);
    }

    glLinkProgram(shader_id);

    if (!validate_program(shader_id))
    {
        fprintf(stderr, "Error while validating shader.\n");
        glfwTerminate();

        glDeleteVertexArrays(1, &vao);
        delete[] buffer.data;

        return EXIT_FAILURE;
    }

    glUseProgram(shader_id);

    GLint location = glGetUniformLocation(shader_id, "buffer");
    glUniform1i(location, 0);

    // OpenGL setup
    glDisable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(vao);

    // Prepare game
    Sprite alien_sprites[6];
    generate_alien_death_sprites(alien_sprites);

    Sprite alien_death_sprite;
    generate_alien_death_sprites(&alien_death_sprite);

    Sprite player_sprite;
    generate_player_sprite(&player_sprite);

    Sprite text_spritesheet;
    generate_spritesheet(&text_spritesheet);

    Sprite number_spritesheet = text_spritesheet;
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

    size_t alien_update_frequency = 120;

    for (size_t i = 0; i < 3; ++i)
    {
        alien_animation[i].loop = true;
        alien_animation[i].num_frames = 2;
        alien_animation[i].frame_duration = alien_update_frequency;
        alien_animation[i].time = 0;

        alien_animation[i].frames = new Sprite *[2];
        alien_animation[i].frames[0] = &alien_sprites[2 * i];
        alien_animation[i].frames[1] = &alien_sprites[2 * i + 1];
    }

    Game game;
    game.width = buffer_width;
    game.height = buffer_height;
    game.num_bullets = 0;
    game.num_aliens = 55;
    game.aliens = new Alien[game.num_aliens];

    game.player->x = 112 - 5;
    game.player->y = 32;
    game.player->lives = 3;

    size_t alien_swarm_position = 24;
    size_t alien_swarm_max_position = game.width - 16 * 11 - 3;

    size_t aliens_killed = 0;
    size_t alien_update_timer = 0;
    bool should_change_speed = false;

    for (size_t xi = 0; xi < 11; ++xi)
    {
        for (size_t yi = 0; yi < 5; ++yi)
        {
            Alien &alien = game.aliens[xi * 5 + yi];
            alien.type = (5 - yi) / 2 + 1;

            const Sprite &sprite = alien_sprites[2 * (alien.type - 1)];

            alien.x = 16 * xi + alien_swarm_position +
                      (alien_death_sprite.width - sprite.width) / 2;

            alien.y = 17 * yi + 128;
        }
    }

    uint8_t *death_counters = new uint8_t[game.num_aliens];

    for (size_t i = 0; i < game.num_aliens; ++i)
        death_counters[i] = 10;

    uint32_t clear_color = rgb_to_uint32(0, 128, 0);
    uint32_t rng = 13;

    int alien_move_dir = 4;

    size_t score = 0;
    size_t credits = 0;

    game_running = true;

    int player_move_dir = 0;
    while (!glfwWindowShouldClose(window.get_window()) && game_running)
    {
        buffer_clear(&buffer, clear_color);

        if (game.player->lives == 0)
        {

            buffer_draw_string(&buffer, text_spritesheet, "GAME OVER",
                               game.width / 2 - 30, game.height / 2,
                               rgb_to_uint32(128, 0, 0));

            buffer_draw_string(&buffer, text_spritesheet, "SCORE",
                               4, game.height - text_spritesheet.height - 7,
                               rgb_to_uint32(128, 0, 0));

            buffer_draw_number(&buffer, number_spritesheet, score,
                               4 + 2 * number_spritesheet.width,
                               game.height - 2 * number_spritesheet.height - 12,
                               rgb_to_uint32(128, 0, 0));

            glTexSubImage2D(
                GL_TEXTURE_2D, 0, 0, 0,
                buffer.width, buffer.height,
                GL_RGBA, GL_UNSIGNED_INT_8_8_8_8,
                buffer.data);

            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            glfwSwapBuffers(window.get_window());
            glfwPollEvents();

            continue;
        }

        // Draw
        buffer_draw_string(&buffer, text_spritesheet, "SCORE",
                           4, game.height - text_spritesheet.height - 7, rgb_to_uint32(128, 0, 0));

        buffer_draw_number(&buffer, number_spritesheet, score,
                           4 + 2 * number_spritesheet.width,
                           game.height - 2 * number_spritesheet.height - 12, rgb_to_uint32(128, 0, 0));

        {
            char credit_text[16];
            sprintf(credit_text, "CREDIT %02lu", credits);

            buffer_draw_string(&buffer, text_spritesheet, credit_text, 164, 7, rgb_to_uint32(128, 0, 0));
        }

        buffer_draw_number(&buffer, number_spritesheet,
                           game.player->lives,
                           4, 7, rgb_to_uint32(128, 0, 0));

        size_t xp = 11 + number_spritesheet.width;

        for (size_t i = 0; i < game.player->lives - 1; ++i)
        {
            buffer_draw_sprite(&buffer, player_sprite, xp, 7,
                               rgb_to_uint32(128, 0, 0));

            xp += player_sprite.width + 2;
        }

        for (size_t i = 0; i < game.width; ++i)
            buffer.data[game.width * 16 + i] = rgb_to_uint32(128, 0, 0);

        for (size_t ai = 0; ai < game.num_aliens; ++ai)
        {
            if (death_counters[ai] == 0)
                continue;

            const Alien &alien = game.aliens[ai];

            if (alien.type == ALIEN_DEAD)
            {
                buffer_draw_sprite(&buffer, alien_death_sprite, alien.x,
                                   alien.y, rgb_to_uint32(128, 0, 0));
            }

            else
            {
                const SpriteAnimation &animation =
                    alien_animation[alien.type - 1];

                size_t current_frame =
                    animation.time / animation.frame_duration;
                const Sprite &sprite = *animation.frames[current_frame];

                buffer_draw_sprite(&buffer, sprite, alien.x, alien.y, rgb_to_uint32(128, 0, 0));
            }
        }

        for (size_t bi = 0; bi < game.num_bullets; ++bi)
        {
            const Bullet &bullet = game.bullets[bi];
            const Sprite *sprite;

            if (bullet.direction > 0)
                sprite = &player_bullet_sprite;

            else
            {
                size_t cf = alien_bullet_animation.time / alien_bullet_animation.frame_duration;
                sprite = &alien_bullet_sprite[cf];
            }

            buffer_draw_sprite(&buffer, *sprite, bullet.x, bullet.y, rgb_to_uint32(128, 0, 0));
        }

        buffer_draw_sprite(&buffer, player_sprite, game.player->x,
                           game.player->y, rgb_to_uint32(128, 0, 0));

        glTexSubImage2D(
            GL_TEXTURE_2D, 0, 0, 0,
            buffer.width, buffer.height,
            GL_RGBA, GL_UNSIGNED_INT_8_8_8_8,
            buffer.data);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glfwSwapBuffers(window.get_window());

        // Simulate bullets
        for (size_t bi = 0; bi < game.num_bullets; ++bi)
        {
            game.bullets[bi].y += game.bullets[bi].direction;

            if (game.bullets[bi].y >= game.height ||
                game.bullets[bi].y < player_bullet_sprite.height)
            {
                game.bullets[bi] = game.bullets[game.num_bullets - 1];
                --game.num_bullets;

                continue;
            }

            // Alien bullet
            if (game.bullets[bi].direction < 0)
            {
                bool overlap = sprite_collision(
                    alien_bullet_sprite[0], game.bullets[bi].x,
                    game.bullets[bi].y, player_sprite, game.player->x,
                    game.player->y);

                if (overlap)
                {
                    --game.player->lives;
                    game.bullets[bi] = game.bullets[game.num_bullets - 1];

                    --game.num_bullets;

                    break;
                }
            }

            // Player bullet
            else
            {
                for (size_t bj = 0; bj < game.num_bullets; ++bj)
                {
                    if (bi == bj)
                        continue;

                    bool overlap = sprite_collision(
                        player_bullet_sprite, game.bullets[bi].x,
                        game.bullets[bi].y, alien_bullet_sprite[0],
                        game.bullets[bj].x, game.bullets[bj].y);

                    if (overlap)
                    {
                        if (bj == game.num_bullets - 1)
                            game.bullets[bi] =
                                game.bullets[game.num_bullets - 2];

                        else if (bi == game.num_bullets - 1)
                            game.bullets[bj] =
                                game.bullets[game.num_bullets - 2];

                        else
                        {
                            game.bullets[(bi < bj) ? bi : bj] =
                                game.bullets[game.num_bullets - 1];

                            game.bullets[(bi < bj) ? bj : bi] =
                                game.bullets[game.num_bullets - 2];
                        }

                        game.num_bullets -= 2;
                        break;
                    }
                }

                // Check hit
                for (size_t ai = 0; ai < game.num_aliens; ++ai)
                {
                    const Alien &alien = game.aliens[ai];

                    if (alien.type == ALIEN_DEAD)
                        continue;

                    const SpriteAnimation &animation =
                        alien_animation[alien.type - 1];

                    size_t current_frame =
                        animation.time / animation.frame_duration;

                    const Sprite &alien_sprite =
                        *animation.frames[current_frame];

                    bool overlap = sprite_collision(
                        player_bullet_sprite, game.bullets[bi].x,
                        game.bullets[bi].y, alien_sprite, alien.x, alien.y);

                    if (overlap)
                    {
                        score += 10 * (4 - game.aliens[ai].type);
                        game.aliens[ai].type = ALIEN_DEAD;

                        game.aliens[ai].x -=
                            (alien_death_sprite.width - alien_sprite.width) / 2;

                        game.bullets[bi] =
                            game.bullets[game.num_bullets - 1];

                        --game.num_bullets;
                        ++aliens_killed;

                        if (aliens_killed % 15 == 0)
                            should_change_speed = true;

                        break;
                    }
                }
            }
        }

        // Simulate aliens
        if (should_change_speed)
        {
            should_change_speed = false;
            alien_update_frequency /= 2;

            for (size_t i = 0; i < 3; ++i)
                alien_animation[i].frame_duration = alien_update_frequency;
        }

        // Update death counters
        for (size_t ai = 0; ai < game.num_aliens; ++ai)
        {
            const Alien &alien = game.aliens[ai];

            if (alien.type == ALIEN_DEAD && death_counters[ai])
                --death_counters[ai];
        }

        if (alien_update_timer >= alien_update_frequency)
        {
            alien_update_timer = 0;

            if (static_cast<int>(alien_swarm_position + alien_move_dir) < 0)
            {
                alien_move_dir *= -1;

                for (size_t ai = 0; ai < game.num_aliens; ++ai)
                {
                    Alien &alien = game.aliens[ai];
                    alien.y -= 8;
                }
            }

            else if (alien_swarm_position >
                     alien_swarm_max_position - alien_move_dir)
                alien_move_dir *= -1;

            alien_swarm_position += alien_move_dir;

            for (size_t ai = 0; ai < game.num_aliens; ++ai)
            {
                Alien &alien = game.aliens[ai];
                alien.x += alien_move_dir;
            }

            if (aliens_killed < game.num_aliens)
            {
                size_t rai = game.num_aliens * random(&rng);

                while (game.aliens[rai].type == ALIEN_DEAD)
                    rai = game.num_aliens * random(&rng);

                const Sprite &alien_sprite =
                    *alien_animation[game.aliens[rai].type - 1].frames[0];

                game.bullets[game.num_bullets].x =
                    game.aliens[rai].x + alien_sprite.width / 2;

                game.bullets[game.num_bullets].y =
                    game.aliens[rai].y - alien_bullet_sprite[0].height;

                game.bullets[game.num_bullets].direction = -2;
                ++game.num_bullets;
            }
        }

        // Update animations
        for (size_t i = 0; i < 3; ++i)
        {
            ++alien_animation[i].time;

            if (alien_animation[i].time >=
                alien_animation[i].num_frames *
                    alien_animation[i].frame_duration)
                alien_animation[i].time = 0;
        }

        ++alien_bullet_animation.time;

        if (alien_bullet_animation.time >=
            alien_bullet_animation.num_frames *
                alien_bullet_animation.frame_duration)
            alien_bullet_animation.time = 0;

        ++alien_update_timer;

        // Simulate player
        player_move_dir = 2 * move_dir;

        if (player_move_dir != 0)
        {
            if (game.player->x + player_sprite.width + player_move_dir >=
                game.width)
                game.player->x = game.width - player_sprite.width;

            else if (static_cast<int>(game.player->x + player_move_dir) <= 0)
                game.player->x = 0;

            else
                game.player->x += player_move_dir;
        }

        if (aliens_killed < game.num_aliens)
        {
            size_t ai = 0;

            while (game.aliens[ai].type == ALIEN_DEAD)
                ++ai;

            const Sprite &sprite =
                alien_sprites[2 * (game.aliens[ai].type - 1)];

            size_t pos =
                game.aliens[ai].x -
                (alien_death_sprite.width - sprite.width) / 2;

            if (pos > alien_swarm_position)
                alien_swarm_position = pos;

            ai = game.num_aliens - 1;

            while (game.aliens[ai].type == ALIEN_DEAD)
                --ai;

            pos = game.width - game.aliens[ai].x - 13 + pos;

            if (pos > alien_swarm_max_position)
                alien_swarm_max_position = pos;
        }

        else
        {
            alien_update_frequency = 120;
            alien_swarm_position = 24;

            aliens_killed = 0;
            alien_update_timer = 0;

            alien_move_dir = 4;

            for (size_t xi = 0; xi < 11; ++xi)
            {
                for (size_t yi = 0; yi < 5; ++yi)
                {
                    size_t ai = xi * 5 + yi;

                    death_counters[ai] = 10;

                    Alien &alien = game.aliens[ai];
                    alien.type = (5 - yi) / 2 + 1;

                    const Sprite &sprite = alien_sprites[2 * (alien.type - 1)];

                    alien.x = 16 * xi + alien_swarm_position +
                              (alien_death_sprite.width - sprite.width) / 2;

                    alien.y = 17 * yi + 128;
                }
            }
        }

        // Process events
        if (fire_pressed && game.num_bullets < MAX_BULLETS)
        {
            game.bullets[game.num_bullets].x =
                game.player->x + player_sprite.width / 2;

            game.bullets[game.num_bullets].y =
                game.player->y + player_sprite.height;

            game.bullets[game.num_bullets].direction = 2;
            ++game.num_bullets;
        }

        fire_pressed = false;

        glfwPollEvents();
    }

    glfwDestroyWindow(window.get_window());
    glfwTerminate();

    glDeleteVertexArrays(1, &vao);

    for (size_t i = 0; i < 6; ++i)
        delete[] alien_sprites[i].data;

    delete[] text_spritesheet.data;
    delete[] alien_death_sprite.data;
    delete[] player_bullet_sprite.data;
    delete[] alien_bullet_sprite[0].data;
    delete[] alien_bullet_sprite[1].data;
    delete[] alien_bullet_animation.frames;

    for (size_t i = 0; i < 3; ++i)
        delete[] alien_animation[i].frames;

    delete[] buffer.data;
    delete[] game.aliens;
    delete[] death_counters;

    return 0;
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
            move_dir += 1;

        else if (action == GLFW_RELEASE)
            move_dir -= 1;

        break;

    case GLFW_KEY_D:
        if (action == GLFW_PRESS)
            move_dir -= 1;

        else if (action == GLFW_RELEASE)
            move_dir += 1;

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
void buffer_clear(Buffer *buffer, uint32_t color)
{
    for (size_t i = 0; i < buffer->width * buffer->height; ++i)
        buffer->data[i] = color;
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
    for (size_t xi = 0; xi < sprite.width; ++xi)
    {
        for (size_t yi = 0; yi < sprite.height; ++yi)
        {
            if (sprite.data[yi * sprite.width + xi] &&
                (sprite.height - 1 + y - yi) < buffer->height &&
                (x + xi) < buffer->width)
                buffer->data[(sprite.height - 1 + y - yi) * buffer->width + (x + xi)] = color;
        }
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