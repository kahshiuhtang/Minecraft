#ifndef MCRFT_RENDERER_HPP
#define MCRFT_RENDERER_HPP

#include <filesystem>
#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "renderer/shader.hpp"
#include "renderer/model.hpp"
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
        int setuptextures();
        int destroyrenderer();
        int rendercrosshair();
        int loop();
        int rendermapmeshes();
        int setupshaders();

    private:
        MCRFT::ShaderManager m_shader_mang;
        std::shared_ptr<MCRFT::Shader> m_curr_shader;
        Camera *m_camera;
        Screen *m_screen;
        World *m_world;
        Gui *m_gui;
        TextureManager *m_texture_manager;
        unsigned int p_VBO;
        unsigned int p_VAO;
        unsigned int norm_shader;
        unsigned int crosshair_shader;
        // load and create a texture
        // -------------------------
        GLuint m_crosshairTextureID;
        int m_crosshairWidth, m_crosshairHeight, m_crosshairChannels;
    };
}
#endif