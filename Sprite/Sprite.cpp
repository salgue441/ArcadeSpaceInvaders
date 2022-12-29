/**
 * @file Sprite.cpp
 * @author Carlos Salguero
 * @brief Implementation of the Sprite class
 * @version 0.1
 * @date 2022-12-28
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "Sprite.h"

// Getters
/**
 * @brief
 * Gets the height of the sprite
 * @return size_t width of the sprite
 */
size_t Sprite::get_width() const
{
    return m_width;
}

/**
 * @brief
 * Gets the height of the sprite
 * @return size_t height of the sprite
 */
size_t Sprite::get_height() const
{
    return m_height;
}

/**
 * @brief
 * Gets the pixels of the sprite
 * @return uint32_t* pixels of the sprite
 */
uint8_t *Sprite::get_pixels() const
{
    return m_pixels;
}

// Setters
/**
 * @brief
 * Sets the width of the sprite
 * @param width width of the sprite
 */
void Sprite::set_width(size_t width)
{
    m_width = width;
}

/**
 * @brief
 * Sets the height of the sprite
 * @param height height of the sprite
 */
void Sprite::set_height(size_t height)
{
    m_height = height;
}

/**
 * @brief
 * Sets the pixels of the sprite
 * @param pixels pixels of the sprite
 */
void Sprite::set_pixels(uint8_t *pixels)
{
    m_pixels = pixels;
}
