/**
 * @file Player.cpp
 * @author Carlos Salguero
 * @brief Implementation of the Player class
 * @version 0.1
 * @date 2022-12-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "Player.h"

// Constructor
/**
 * @brief
 * Construct a new Player:: Player object
 * @param x X position of the player
 * @param y Y position of the player
 * @param life Life of the player
 */
Player::Player(size_t x, size_t y, size_t life)
{
    this->x = x;
    this->y = y;
    this->life = life;
}

// Getters
/**
 * @brief
 * Get the X position of the player
 * @return size_t X position of the player
 */
size_t Player::get_x() const
{
    return this->x;
}

/**
 * @brief
 * Get the Y position of the player
 * @return size_t Y position of the player
 */
size_t Player::get_y() const
{
    return this->y;
}

/**
 * @brief
 * Get the life of the player
 * @return size_t Life of the player
 */
size_t Player::get_life() const
{
    return this->life;
}

// Setters
/**
 * @brief
 * Set the X position of the player
 * @param x X position of the player
 */
void Player::set_x(size_t &x)
{
    this->x = x;
}

/**
 * @brief
 * Set the Y position of the player
 * @param y Y position of the player
 */
void Player::set_y(size_t &y)
{
    this->y = y;
}

/**
 * @brief
 * Set the life of the player
 * @param life Life of the player
 */
void Player::set_life(size_t &life)
{
    this->life = life;
}
