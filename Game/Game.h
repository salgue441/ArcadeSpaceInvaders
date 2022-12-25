/**
 * @file Game.cpp
 * @author Carlos Salguero
 * @brief Declaration of the Game class
 * @version 0.1
 * @date 2022-12-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef GAME_H
#define GAME_H

#define MAX_BULLETS 128

#include <cstdint>

// Custom headers
#include "../Player/Player.cpp"
#include "../Bullet/Bullet.cpp"

class Game
{
public:
    // Constructor
    Game(size_t, size_t);

    // Destructor
    ~Game() = default;

    // Getters
    size_t get_width() const;
    size_t get_height() const;
    size_t get_num_aliens() const;

    Player &get_player();
    Bullet &get_bullet();

    // Setters
    void set_width(size_t &);
    void set_height(size_t &);
    void set_num_aliens(size_t &);
    void set_num_bullets(size_t &);

private:
    // Game objects
    Player player;
    Bullet bullets[MAX_BULLETS];

    // Game variables
    size_t width{}, height{};
    size_t num_aliens{};
    size_t num_bullets{};
};

#endif //! GAME_H
