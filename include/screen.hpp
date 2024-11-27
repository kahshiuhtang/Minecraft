#ifndef MCRFT_SCREEN_HPP
#define MCRFT_SCREEN_HPP

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "camera.hpp"
namespace MCRFT
{
    // settings
    const unsigned int SCR_WIDTH = 800;
    const unsigned int SCR_HEIGHT = 600;
    class Screen
    {
    public:
        int setupwindow();
        int setupinputcallbacks();

        GLFWwindow *m_window;
        unsigned int getscreenwidth(){ return SCR_WIDTH; }
        unsigned int getscreenheight(){ return SCR_HEIGHT; }

    private:
    };
}
#endif