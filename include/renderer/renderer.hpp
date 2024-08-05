#ifndef MCRFT_RENDERER_HPP
#define MCRFT_RENDERER_HPP

#include <filesystem>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "renderer/shader.hpp"
#include "renderer/gui.hpp"
#include "renderer/texture.hpp"
#include "world/world.hpp"
#include "screen.hpp"
#include "camera.hpp"

namespace MCRFT
{
    class Renderer
    {
    public:
        Renderer();
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
        Gui *m_gui;
        TextureManager *m_texture_manager;
        unsigned int p_VBO;
        unsigned int p_VAO;
        // load and create a texture
        // -------------------------
        GLuint m_crosshairTextureID;
        int m_crosshairWidth, m_crosshairHeight, m_crosshairChannels;
    };
}
#endif