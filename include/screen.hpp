#ifndef MCRFT_SCREEN_HPP
#define MCRFT_SCREEN_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "camera.hpp"
namespace MCRFT
{
    // settings
    const unsigned int SCR_WIDTH = 800;
    const unsigned int SCR_HEIGHT = 600;
    class Screen
    {
    public:
        int init_window();
        int init_callbacks();

        GLFWwindow *m_window;

    private:
    };
}
#endif