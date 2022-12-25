/**
 * @file Window.h
 * @author Carlos Salguero
 * @brief Definition of the Window class
 * @version 0.1
 * @date 2022-12-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef WINDOW_H
#define WINDOW_H

#include <string_view>
#include <string>
#include <stdlib.h>
#include <stdexcept>

// OpenGL Headers
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// OpenGL Error
#include "../Error/glError.h"

class Window
{
public:
    // Constructor
    Window(size_t, size_t, std::string_view);

    // Destructor
    ~Window() = default;

    // Getters
    const size_t getWidth() const;
    const size_t getHeight() const;
    const std::string getTitle() const;
    GLFWwindow *getWindow() const;

    // Functions
    void init();
    void update();
    void clear() const;
    void show_version() const;
    void terminate();

private:
    size_t m_width;
    size_t m_height;
    std::string m_title;
    GLFWwindow *m_window;
};

#endif //! WINDOW_H