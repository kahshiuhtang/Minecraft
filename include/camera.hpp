#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"

namespace MCRFT
{
    class Camera
    {
    private:
        // settings
        const unsigned int SCR_WIDTH = 800;
        const unsigned int SCR_HEIGHT = 600;

    public:
        void init_camera(GLFWwindow *m_window);
        void update_frame();
        void process_input(GLFWwindow *window);
        void update_shaders_projection_mat(Shader *m_shader);

        // timing
        float m_delta_time = 0.0f; // time between current frame and last frame
        float m_last_frame = 0.0f;

        // camera
        glm::vec3 m_camera_pos = glm::vec3(0.0f, 0.0f, 3.0f);
        glm::vec3 m_camera_front = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 m_camera_up = glm::vec3(0.0f, 1.0f, 0.0f);

        bool m_first_mouse = true;
        float m_yaw = -90.0f; // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
        float m_pitch = 0.0f;
        float m_last_x = 800.0f / 2.0;
        float m_last_y = 600.0 / 2.0;
        float m_fov = 45.0f;
    };
}
#endif