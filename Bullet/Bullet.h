/**
 * @file Bullet.h
 * @author Carlos Salguero
 * @brief Declaration of the Bullet class
 * @version 0.1
 * @date 2022-12-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef BULLET_H
#define BULLET_H

#include <stdio.h>

class Bullet
{
public:
    // Constructor
    Bullet() = default;
    Bullet(size_t, size_t);
    Bullet(size_t, size_t, int);

    // Destructor
    ~Bullet() = default;

    // Getters
    size_t get_x() const;
    size_t get_y() const;
    int get_direction() const;

    // Setters
    void set_x(size_t &);
    void set_y(size_t &);
    void set_direction(int &);

private:
    // Bullet variables
    size_t x{}, y{};
    int direction{};
};

#endif //! BULLET_H