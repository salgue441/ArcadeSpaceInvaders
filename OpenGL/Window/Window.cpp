/**
 * @file Window.cpp
 * @author Carlos Salguero
 * @brief Implementation of the Window class
 * @version 0.1
 * @date 2022-12-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "Window.h"

// Constructor
/**
 * @brief
 * Construct a new Window:: Window object
 * @param width Width of the window
 * @param height Height of the window
 * @param title Title of the window
 */
Window::Window(size_t width, size_t height, std::string_view title)
{
    this->m_width = width;
    this->m_height = height;
    this->m_title = title;
    this->m_window = nullptr;

    init();
}

// Getters
/**
 * @brief
 * Get the Width of the window
 * @return const size_t Width of the window
 */
const size_t Window::get_width() const
{
    return this->m_width;
}

/**
 * @brief
 * Get the Height of the window
 * @return const size_t Height of the window
 */
const size_t Window::get_height() const
{
    return this->m_height;
}

/**
 * @brief
 * Get the Title of the window
 * @return const std::string Title of the window
 */
const std::string Window::get_title() const
{
    return this->m_title;
}

/**
 * @brief
 * Get the Window object
 * @return GLFWwindow* Pointer to the window
 */
GLFWwindow *Window::get_window() const
{
    return this->m_window;
}

// Functions
/**
 * @brief
 * Initialize the window
 */
void Window::init()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    // Set OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create the window
    this->m_window = glfwCreateWindow(2 * this->m_width, 2 * this->m_height,
                                      this->m_title.c_str(),
                                      NULL, NULL);

    if (!this->m_window)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create the window");
    }

    // Make the window's context current
    glfwMakeContextCurrent(this->m_window);

    // Initialize GLEW
    if (glewInit() != GLEW_OK)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to initialize GLEW");
    }

    this->show_version();
}

/**
 * @brief
 * Update the window
 */
void Window::update()
{
    // Swap front and back buffers
    glfwSwapBuffers(this->m_window);

    // Poll for and process events
    glfwPollEvents();
}

/**
 * @brief
 * Clear the window
 */
void Window::clear() const
{
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/**
 * @brief
 * Show the version of OpenGL
 */
void Window::show_version() const
{
    int glVersion[2] = {-1, -1};

    glGetIntegerv(GL_MAJOR_VERSION, &glVersion[0]);
    glGetIntegerv(GL_MINOR_VERSION, &glVersion[1]);

    gl_debug(__FILE__, __LINE__);

    printf("OpenGL version: %d.%d\n", glVersion[0], glVersion[1]);
    printf("OpenGL Renderer: %s\n", glGetString(GL_RENDERER));
    printf("Shading Language Version: %s\n",
           glGetString(GL_SHADING_LANGUAGE_VERSION));
    printf("Vendor: %s\n", glGetString(GL_VENDOR));
    printf("GLFW version: %s\n", glfwGetVersionString());
}

/**
 * @brief
 * Terminate the window
 */
void Window::terminate()
{
    glfwDestroyWindow(this->m_window);
    glfwTerminate();
}