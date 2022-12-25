/**
 * @file Player.h
 * @author Carlos Salguero
 * @brief Declaration of the Player class
 * @version 0.1
 * @date 2022-12-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef PLAYER_H
#define PLAYER_H

#include <cstdint>
#include <cstdio>

class Player
{
public:
    // Constructor
    Player() = default;
    Player(size_t, size_t, size_t);

    // Destructor
    ~Player() = default;

    // Getters
    size_t get_x() const;
    size_t get_y() const;
    size_t get_life() const;

    // Setters
    void set_x(size_t);
    void set_y(size_t);
    void set_life(size_t);

private:
    size_t x, y;
    size_t life;
};

#endif //! PLAYER_H