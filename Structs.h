/**
 * @file Structs.h
 * @author Carlos Salguero
 * @brief Contains all the structs used in the game
 * @version 0.1
 * @date 2023-02-02
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef STRUCTS_H
#define STRUCTS_H

// C++ Standard Library
#include <cstddef>
#include <cstdint>

// Structs
/**
 * @brief
 * Struct that contains the width and height of the buffer
 * and a pointer to the data of the buffer
 */
struct Buffer
{
    size_t width, height;
    uint32_t *data;
};

/**
 * @brief
 * Struct that contains the width and height of the sprite
 * and a pointer to the data of the sprite
 */
struct Sprite
{
    std::size_t width, height;
    uint8_t *data;
};

/**
 * @brief
 * Struct that contains the x and y position of the sprite
 * and the type of the sprite
 */
struct Alien
{
    std::size_t x, y;
    uint8_t type;
};

/**
 * @brief
 * Struct that contains the x and y position of the sprite
 * and the direction of the sprite
 */
struct Bullet
{
    std::size_t x, y;
    int direction;
};

/**
 * @brief
 * Struct that contains the x and y position of the sprite
 * and the number of lives of the player
 */
struct Player
{
    std::size_t x, y;
    std::size_t lives;
};

#define MAX_BULLETS 128

/**
 * @brief
 * Struct that contains the width and height of the game,
 * the number of aliens and bullets, a pointer to the aliens,
 * a pointer to the player and an array of bullets.
 */
struct Game
{
    std::size_t width, height;
    std::size_t num_aliens;
    std::size_t num_bullets;

    Alien *aliens;
    Player *player;
    Bullet bullets[MAX_BULLETS];
};

/**
 * @brief
 * Struct that contains the number of frames, the duration of each frame,
 * the time of the animation and a pointer to the frames of the animation
 */
struct SpriteAnimation
{
    bool loop;
    std::size_t num_frames;
    std::size_t frame_duration;
    std::size_t time;

    Sprite **frames;
};

// Enums
/**
 * @brief
 * Enum that contains the different types of aliens in the game
 */
enum AlienType : uint8_t
{
    ALIEN_DEAD = 0,
    ALIEN_TYPE_A = 1,
    ALIEN_TYPE_B = 2,
    ALIEN_TYPE_C = 3,
};

#endif //! STRUCTS_H