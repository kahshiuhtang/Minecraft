#ifndef SCREEN_HPP
#define SCREEN_HPP

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

        int finish_init();
        GLFWwindow *m_window;

    private:
    };
}
#endif