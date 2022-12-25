/**
 * @file Alien.cpp
 * @author Carlos Salguero
 * @brief Implementation of the Alien class
 * @version 0.1
 * @date 2022-12-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "Alien.h"

// Constructor
/**
 * @brief
 * Construct a new Alien:: Alien object
 * @param x X position of the alien
 * @param y Y position of the alien
 * @param type Type of the alien
 */
Alien::Alien(size_t x, size_t y, uint8_t type)
{
    this->x = x;
    this->y = y;
    this->type = type;
}

// Getters
/**
 * @brief
 * Get the x position of the alien
 * @return size_t X position of the alien
 */
size_t Alien::get_x() const
{
    return this->x;
}

/**
 * @brief
 * Get the y position of the alien
 * @return size_t Y position of the alien
 */
size_t Alien::get_y() const
{
    return this->y;
}

/**
 * @brief
 * Get the type of the alien
 * @return uint8_t Type of the alien
 */
uint8_t Alien::get_type() const
{
    return this->type;
}

// Setters
/**
 * @brief
 * Set the x position of the alien
 * @param x X position of the alien
 */
void Alien::set_x(size_t x)
{
    this->x = x;
}

/**
 * @brief
 * Set the y position of the alien
 * @param y Y position of the alien
 */
void Alien::set_y(size_t y)
{
    this->y = y;
}

/**
 * @brief
 * Set the type of the alien
 * @param type Type of the alien
 */
void Alien::set_type(uint8_t type)
{
    this->type = type;
}