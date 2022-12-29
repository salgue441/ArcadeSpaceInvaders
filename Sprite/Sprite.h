/**
 * @file Sprite.h
 * @author Carlos Salguero
 * @brief Sprite class declaration
 * @version 0.1
 * @date 2022-12-28
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
    // Constructors
    Sprite() = default;

    // Destructor
    ~Sprite() = default;

    // Getters
    size_t get_width() const;
    size_t get_height() const;
    uint8_t *get_pixels() const;

    // Setters
    void set_width(size_t);
    void set_height(size_t);
    void set_pixels(uint8_t *);

private:
    size_t m_width{}, m_height{};
    uint8_t *m_pixels{nullptr};
};

#endif //! SPRITE_H