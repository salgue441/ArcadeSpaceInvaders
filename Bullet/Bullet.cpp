/**
 * @file Bullet.cpp
 * @author Carlos Salguero
 * @brief Implementation of the Bullet class
 * @version 0.1
 * @date 2022-12-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "Bullet.h"

// Constructor
/**
 * @brief
 * Construct a new Bullet:: Bullet object
 * @param x x coordinate
 * @param y y coordinate
 * @param direction direction of the bullet
 */
Bullet::Bullet(size_t x, size_t y, int direction)
{
    this->x = x;
    this->y = y;
    this->direction = direction;
}

// Getters
/**
 * @brief
 * Get the x coordinate
 * @return size_t x coordinate
 */
size_t Bullet::get_x() const
{
    return this->x;
}

/**
 * @brief
 * Get the y coordinate
 * @return size_t y coordinate
 */
size_t Bullet::get_y() const
{
    return this->y;
}

/**
 * @brief
 * Get the direction
 * @return int direction
 */
int Bullet::get_direction() const
{
    return this->direction;
}

// Setters
/**
 * @brief
 * Set the x coordinate
 * @param x x coordinate
 */
void Bullet::set_x(size_t &x)
{
    this->x = x;
}

/**
 * @brief
 * Set the y coordinate
 * @param y y coordinate
 */
void Bullet::set_y(size_t &y)
{
    this->y = y;
}

/**
 * @brief
 * Set the direction
 * @param direction direction
 */
void Bullet::set_direction(int &direction)
{
    this->direction = direction;
}