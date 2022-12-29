/**
 * @file Alien.h
 * @author Carlos Salguero
 * @brief Declaration of the Alien class
 * @version 0.1
 * @date 2022-12-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef ALIEN_H
#define ALIEN_H

#include <stdio.h>
#include <cstdint>

class Alien
{
public:
    // Constructor
    Alien() = default;

    // Destructor
    ~Alien() = default;

    // Getters
    size_t get_x() const;
    size_t get_y() const;
    uint8_t get_type() const;

    // Setters
    void set_x(size_t);
    void set_y(size_t);
    void set_type(uint8_t);

private:
    // Alien variables
    size_t x{}, y{};
    uint8_t type;
};

#endif //! ALIEN_H