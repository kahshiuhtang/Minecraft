#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <filesystem>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "screen.hpp"
#include "shader.hpp"
#include "camera.hpp"

namespace MCRFT
{
    class Renderer
    {
    public:
        int init();
        int init_textures();
        int loop();
        int setup_shaders();
        int destroy_renderer();
        int _render_crosshair();

    private:
        Shader *m_shader;
        Camera *m_camera;
        Screen *m_screen;
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