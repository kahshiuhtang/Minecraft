#include "renderer/gui.hpp"

MCRFT::Gui::Gui(GLFWwindow *window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}
void MCRFT::Gui::startnewframe()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}
void MCRFT::Gui::renderframe(const char *fps_string)
{
    ImGui::Begin("FPS");
    // Text that appears in the window
    if (fps_string == nullptr)
    {
        ImGui::Text("LOADING");
    }
    else
    {
        ImGui::Text(fps_string);
    }
    // Ends the window
    ImGui::End();
    // Renders the ImGUI elements
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}