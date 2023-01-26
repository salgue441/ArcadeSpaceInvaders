/**
 * @file Sprite.h
 * @author Carlos Salguero
 * @brief Declaration of the Sprite Class
 * @version 0.1
 * @date 2023-01-25
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef SPRITE_H
#define SPRITE_H

#include <cstdint>

class Sprite
{
public:
    // Constructor
    Sprite() = default;

    // Destructor
    ~Sprite() = default;

    // Getters
    std::size_t get_sprite_width() const;
    std::size_t get_sprite_height() const;
    uint8_t *get_data();

    // Setters
    void set_sprite_width(std::size_t);
    void set_sprite_height(std::size_t);
    void set_data(uint8_t *);

private:
    std::size_t sprite_width, sprite_height;
    uint8_t *sprite_data;
};

#endif //! SPRITE_H