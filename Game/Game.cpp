/**
 * @file Game.cpp
 * @author Carlos Salguero
 * @brief Implementation of the Game class
 * @version 0.1
 * @date 2022-12-28
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "Game.h"

// Getters
/**
 * @brief
 * Gets the width of the game
 * @return size_t
 */
size_t Game::get_width() const
{
    return width;
}

/**
 * @brief
 * Gets the height of the game
 * @return size_t
 */
size_t Game::get_height() const
{
    return height;
}

/**
 * @brief
 * Gets the number of aliens in the game
 * @return size_t
 */
size_t Game::get_num_aliens() const
{
    return num_aliens;
}

/**
 * @brief
 * Gets the number of bullets in the game
 * @return size_t
 */
size_t Game::get_num_bullets() const
{
    return num_bullets;
}

/**
 * @brief
 * Gets an alien from the game
 * @param index
 * @return Alien&
 */
Alien &Game::get_alien(size_t index) const
{
    return aliens[index];
}

// Setters
/**
 * @brief
 * Sets the width of the game
 * @param width Width of the game
 */
void Game::set_width(size_t width)
{
    this->width = width;
}

/**
 * @brief
 * Sets the height of the game
 * @param height Height of the game
 */
void Game::set_height(size_t height)
{
    this->height = height;
}

/**
 * @brief
 * Sets the number of aliens in the game
 * @param num_aliens Number of aliens
 */
void Game::set_num_aliens(size_t num_aliens)
{
    this->num_aliens = num_aliens;
}

/**
 * @brief
 * Sets the number of bullets in the game
 * @param num_bullets Number of bullets
 */
void Game::set_num_bullets(size_t num_bullets)
{
    this->num_bullets = num_bullets;
}

/**
 * @brief
 * Sets an alien in the game
 * @param index Index of the alien
 * @param alien Alien to set
 */
void Game::set_alien(size_t index, Alien &alien)
{
    aliens[index] = alien;
}