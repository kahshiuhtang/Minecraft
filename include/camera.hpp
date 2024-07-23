#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "player.hpp"
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
        float m_delta_time; // time between current frame and last frame
        float m_last_frame;

        // camera
        glm::vec3 m_camera_pos;
        glm::vec3 m_camera_front;
        glm::vec3 m_camera_up;

        // player
        Player *m_curr_player;

        bool m_first_mouse;
        float m_yaw; // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
        float m_pitch;
        float m_last_x;
        float m_last_y;
        float m_fov;
    };
}
#endif