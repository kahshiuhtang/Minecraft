#ifndef MCRFT_GUI_HPP
#define MCRFT_GUI_HPP

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

namespace MCRFT
{
    class Gui
    {
    public:
        Gui(GLFWwindow *window);
        void startnewframe();
        void renderframe(const char *fps_string);

    private:
    };
}
#endif