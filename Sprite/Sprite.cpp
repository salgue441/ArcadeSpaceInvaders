/**
 * @file Sprite.cpp
 * @author Carlos Salguero
 * @brief Implementation of the Sprite class
 * @version 0.1
 * @date 2022-12-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "Sprite.h"

// Constructor
/**
 * @brief
 * Construct a new Sprite:: Sprite object
 * @param filename File name
 */
Sprite::Sprite() : m_width{0}, m_height{0}, m_pixels{nullptr}
{
}

// Getter
/**
 * @brief
 * Get the pixels of the sprite
 * @return uint8_t* Pointer to the pixels
 */
uint8_t *Sprite::get_pixels() const
{
    return m_pixels;
}

/**
 * @brief
 * Get the width of the sprite
 */
size_t Sprite::get_width() const
{
    return m_width;
}

/**
 * @brief
 * Get the height of the sprite
 */
size_t Sprite::get_height() const
{
    return m_height;
}

// Setter
void Sprite::super_set(size_t width, size_t height, uint8_t *pixels)
{
    this->set_width(width);
    this->set_height(height);
    this->set_pixels(pixels);
}

// Private Setters
/**
 * @brief
 * Set the width of the sprite
 * @param width Width of the sprite
 */
void Sprite::set_width(size_t width)
{
    m_width = width;
}

/**
 * @brief
 * Set the height of the sprite
 * @param height Height of the sprite
 */
void Sprite::set_height(size_t height)
{
    m_height = height;
}

/**
 * @brief
 * Set the pixels of the sprite
 * @param pixels Pointer to the pixels
 */
void Sprite::set_pixels(uint8_t *pixels)
{
    m_pixels = pixels;
}