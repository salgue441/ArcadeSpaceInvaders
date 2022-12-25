/**
 * @file Buffer.h
 * @author Carlos Salguero
 * @brief Definition of the Buffer class
 * @version 0.1
 * @date 2022-12-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef BUFFER_H
#define BUFFER_H

#include <stdio.h>
#include <cstdint>
#include <cstring>

class Buffer
{
public:
    // Constructor
    Buffer(size_t, size_t);

    // Destructor
    ~Buffer() = default;

    // Getters
    const size_t get_width() const;
    const size_t get_height() const;
    uint32_t *get_data();

    // Setters
    void set_width(size_t &);
    void set_height(size_t &);
    void set_data(uint32_t *);

    // Function
    void clear();

private:
    size_t width{}, height{};
    uint32_t *data;
};

#endif //! BUFFER_H