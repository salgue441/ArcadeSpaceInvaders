/**
 * @file Sprite.cpp
 * @author Carlos Salguero
 * @brief Implementation of the Sprite class
 * @version 0.1
 * @date 2023-01-25
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "Sprite.h"

// Getters
/**
 * @brief
 * Gets the height of the sprite
 * @return size_t width of the sprite
 */
std::size_t Sprite::get_sprite_width() const
{
    return sprite_width;
}

/**
 * @brief
 * Gets the height of the sprite
 * @return size_t height of the sprite
 */
std::size_t Sprite::get_sprite_height() const
{
    return sprite_height;
}

/**
 * @brief
 * Gets the pixels of the sprite
 * @return uint32_t* pixels of the sprite
 */
uint8_t *Sprite::get_data()
{
    return sprite_data;
}

// Setters
/**
 * @brief
 * Sets the width of the sprite
 * @param width Width of the sprite
 */
void Sprite::set_sprite_width(std::size_t width)
{
    sprite_width = width;
}

/**
 * @brief
 * Sets the height of the sprite
 * @param height Height of the sprite
 */
void Sprite::set_sprite_height(std::size_t height)
{
    sprite_height = height;
}

/**
 * @brief
 * Sets the pixels of the sprite
 * @param data pixels of the sprite
 */
void Sprite::set_data(uint8_t *data)
{
    sprite_data = data;
}