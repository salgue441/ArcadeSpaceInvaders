/**
 * @file Buffer.cpp
 * @author Carlos Salguero
 * @brief Implementation of the Buffer class
 * @version 0.1
 * @date 2022-12-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "Buffer.h"

// Constructor
/**
 * @brief
 * Construct a new Buffer:: Buffer object
 * @param width Buffer width
 * @param height Buffer height
 */
Buffer::Buffer(size_t width, size_t height)
    : width{width}, height{height}
{
    data = new uint32_t[width * height];
}

// Getters
/**
 * @brief
 * Get the width of the buffer
 * @return const size_t
 */
const size_t Buffer::get_width() const
{
    return width;
}

/**
 * @brief
 * Get the height of the buffer
 * @return const size_t
 */
const size_t Buffer::get_height() const
{
    return height;
}

/**
 * @brief
 * Get the data of the buffer
 * @return const uint32_t*
 */
uint32_t *Buffer::get_data() const
{
    return data;
}

// Setters
/**
 * @brief
 * Set the width of the buffer
 * @param width
 */
void Buffer::set_width(size_t &width)
{
    this->width = width;
}

/**
 * @brief
 * Set the height of the buffer
 * @param height
 */
void Buffer::set_height(size_t &height)
{
    this->height = height;
}

/**
 * @brief
 * Set the data of the buffer
 * @param data
 */
void Buffer::set_data(uint32_t *data)
{
    this->data = data;
}

// Function
/**
 * @brief
 * Clear the buffer
 */
void Buffer::clear()
{
    memset(data, 0, width * height * sizeof(uint32_t));
}
