/**
 * @file Game.cpp
 * @author Carlos Salguero
 * @brief Implementation of the Game class
 * @version 0.1
 * @date 2022-12-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "Game.h"

// Constructor
/**
 * @brief
 * Construct a new Game:: Game object
 * @param width Width of the game
 * @param height Height of the game
 */
Game::Game(size_t width, size_t height)
{
    this->width = width;
    this->height = height;
}

// Getters
/**
 * @brief
 * Get the width of the game
 * @return size_t Width of the game
 */
size_t Game::get_width() const
{
    return this->width;
}

/**
 * @brief
 * Get the height of the game
 * @return size_t Height of the game
 */
size_t Game::get_height() const
{
    return this->height;
}

/**
 * @brief
 * Get the number of aliens
 * @return size_t Number of aliens
 */
size_t Game::get_num_aliens() const
{
    return this->num_aliens;
}

/**
 * @brief
 * Get the player
 * @return Player* Player
 */
Player &Game::get_player()
{
    return player;
}

/**
 * @brief
 * Get the bullet
 * @return Bullet* Bullet
 */
Bullet &Game::get_bullet()
{
    return bullets[0];
}

// Setters
/**
 * @brief
 * Set the width of the game
 * @param width Width of the game
 */
void Game::set_width(size_t &width)
{
    this->width = width;
}

/**
 * @brief
 * Set the height of the game
 * @param height Height of the game
 */
void Game::set_height(size_t &height)
{
    this->height = height;
}

/**
 * @brief
 * Set the number of aliens
 * @param num_aliens Number of aliens
 */
void Game::set_num_aliens(size_t &num_aliens)
{
    this->num_aliens = num_aliens;
}

/**
 * @brief
 * Set the number of bullets
 * @param num_bullets Number of bullets
 */
void Game::set_num_bullets(size_t &num_bullets)
{
    this->num_bullets = num_bullets;
}
