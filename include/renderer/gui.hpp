#ifndef MCRFT_GUI_HPP
#define MCRFT_GUI_HPP

#include "external/imgui/imgui.h"
#include "external/imgui/imgui_impl_glfw.h"
#include "external/imgui/imgui_impl_opengl3.h"

namespace MCRFT
{
    class Gui
    {
    public:
        Gui(GLFWwindow *window);
        void start_new_frame();
        void render_frame(const char *fps_string);

    private:
    };
}
#endif