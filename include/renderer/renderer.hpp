#ifndef MCRFT_RENDERER_HPP
#define MCRFT_RENDERER_HPP

#include <filesystem>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "external/imgui/imgui.h"
#include "external/imgui/imgui_impl_glfw.h"
#include "external/imgui/imgui_impl_opengl3.h"

#include "screen.hpp"
#include "renderer/shader.hpp"
#include "camera.hpp"
#include "world/world.hpp"

namespace MCRFT
{
    class Renderer
    {
    public:
        int init();
        int init_textures();
        int destroy_renderer();
        int _render_crosshair();
        int loop();
        int render_map_meshes();
        int setup_shaders();

    private:
        Shader *m_shader;
        Camera *m_camera;
        Screen *m_screen;
        World *m_world;
        unsigned int p_VBO;
        unsigned int p_VAO;
        // load and create a texture
        // -------------------------
        unsigned int texture1;
        unsigned int texture2;
        GLuint m_crosshairTextureID;
        int m_crosshairWidth, m_crosshairHeight, m_crosshairChannels;
    };
}
#endif