/**
 * @file Game.h
 * @author Carlos Salguero
 * @brief Declaration of the Game class
 * @version 0.1
 * @date 2022-12-28
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef GAME_H
#define GAME_H

#define MAX_BULLETS 128

#include <cstdint>
#include <cstdio>

// Game Classes

class Game
{
public:
    // Constructor
    Game() = default;

    // Destructor
    ~Game() = default;

    // Getters
    size_t get_width() const;
    size_t get_height() const;
    size_t get_num_aliens() const;
    size_t get_num_bullets() const;

    // Setters
    void set_width(size_t width);
    void set_height(size_t height);
    void set_num_aliens(size_t num_aliens);
    void set_num_bullets(size_t num_bullets);

private:
    // Game variables
    size_t width{}, height{};
    size_t num_aliens{};
    size_t num_bullets{};
};

#endif //! GAME_H