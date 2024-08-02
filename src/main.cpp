#include "renderer.hpp"
#include "world.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
int main()
{

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    MCRFT::Renderer *renderer = new MCRFT::Renderer();
    renderer->init();
    renderer->loop();
    renderer->destroy_renderer();
}