/**
 * @file Sprite.h
 * @author Carlos Salguero
 * @brief Declaration of the Sprite class
 * @version 0.1
 * @date 2022-12-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef SPRITE_H
#define SPRITE_H

#include <cstdint>
#include <cstdio>

class Sprite
{
public:
    // Constructor
    Sprite();

    // Destructor
    ~Sprite() = default;

    // Getters
    uint8_t *get_pixels() const;
    size_t get_width() const;
    size_t get_height() const;

    // Setters
    void super_set(size_t, size_t, uint8_t *);

private:
    size_t m_width, m_height;
    uint8_t *m_pixels;

    // Setters
    void set_width(size_t);
    void set_height(size_t);
    void set_pixels(uint8_t *);
};

#endif //! SPRITE_H