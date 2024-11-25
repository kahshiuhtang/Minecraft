#include "camera.hpp"
#include <iostream>
MCRFT::Camera *camera = nullptr;

void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

MCRFT::Camera::Camera(GLFWwindow *m_window)
{
    try
    {
        if (camera == nullptr)
        {
            camera = this;
        }
        m_delta_time = 0.0f; // time between current frame and last frame
        m_last_frame = 0.0f;

        // camera
        m_camera_pos = glm::vec3(0.0f, 100.0f, 0.0f);
        m_camera_front = glm::vec3(0.0f, 0.0f, -1.0f);
        m_camera_up = glm::vec3(0.0f, 1.0f, 0.0f);

        m_first_mouse = true;
        m_yaw = -90.0f; // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
        m_pitch = 0.0f;
        m_last_x = 800.0f / 2.0;
        m_last_y = 600.0 / 2.0;
        m_fov = 45.0f;

        m_curr_player = new MCRFT::Player();
        glfwSetCursorPosCallback(m_window, mouse_callback);
        glfwSetScrollCallback(m_window, scroll_callback);
    }
    catch (const std::exception &e)
    {
        std::cout << "Camera(): Exception: " << e.what() << std::endl;
    }
}
void MCRFT::Camera::updateframe()
{
    // per-frame time logic
    // --------------------
    float current_frame = static_cast<float>(glfwGetTime());
    m_delta_time = current_frame - m_last_frame;
    m_last_frame = current_frame;
}
void MCRFT::Camera::updateprojmatrix(Shader *shader)
{
    try
    {
        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(m_fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        shader->setmat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = glm::lookAt(m_camera_pos, m_camera_pos + m_camera_front, m_camera_up);
        shader->setmat4("view", view);
    }
    catch (const std::exception &e)
    {
        std::cout << "Camera updateprojmatrix(): Exception: " << e.what() << std::endl;
    }
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void MCRFT::Camera::processinput(GLFWwindow *window)
{
    try
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        float camera_speed = static_cast<float>(8.5 * m_delta_time);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            glm::vec3 change = camera_speed * m_camera_front;
            change.y = 0;
            m_curr_player->m_current_pos += change;
            m_camera_pos += change;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {

            glm::vec3 change = camera_speed * m_camera_front;
            change.y = 0;
            m_curr_player->m_current_pos -= change;
            m_camera_pos -= change;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            glm::vec3 change = glm::normalize(glm::cross(m_camera_front, m_camera_up)) * camera_speed;
            change.y = 0;
            m_curr_player->m_current_pos -= change;
            m_camera_pos -= change;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            glm::vec3 change = glm::normalize(glm::cross(m_camera_front, m_camera_up)) * camera_speed;
            change.y = 0;
            m_curr_player->m_current_pos += change;
            m_camera_pos += change;
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            m_curr_player->m_current_pos.y += camera_speed;
            m_camera_pos.y += camera_speed;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        {
            m_curr_player->m_current_pos.y -= camera_speed;
            m_camera_pos.y -= camera_speed;
        }
    }
    catch (const std::exception &e)
    {
        std::cout << "Camera processinput(): Exception: " << e.what() << std::endl;
    }
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    try
    {
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);
        if (camera->m_first_mouse)
        {
            camera->m_last_x = xpos;
            camera->m_last_y = ypos;
            camera->m_first_mouse = false;
        }

        float xoffset = xpos - camera->m_last_x;
        float yoffset = camera->m_last_y - ypos; // reversed since y-coordinates go from bottom to top
        camera->m_last_x = xpos;
        camera->m_last_y = ypos;

        float sensitivity = 0.1f; // change this value to your liking
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        camera->m_yaw += xoffset;
        camera->m_pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (camera->m_pitch > 89.0f)
            camera->m_pitch = 89.0f;
        if (camera->m_pitch < -89.0f)
            camera->m_pitch = -89.0f;

        glm::vec3 front;
        front.x = cos(glm::radians(camera->m_yaw)) * cos(glm::radians(camera->m_pitch));
        front.y = sin(glm::radians(camera->m_pitch));
        front.z = sin(glm::radians(camera->m_yaw)) * cos(glm::radians(camera->m_pitch));
        camera->m_camera_front = glm::normalize(front);
    }
    catch (const std::exception &e)
    {
        std::cout << "mouse_callback(): Exception: " << e.what() << std::endl;
    }
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    try
    {
        if (camera == nullptr)
            return;
        camera->m_fov -= (float)yoffset;
        if (camera->m_fov < 1.0f)
            camera->m_fov = 1.0f;
        if (camera->m_fov > 45.0f)
            camera->m_fov = 45.0f;
    }
    catch (const std::exception &e)
    {
        std::cout << "scroll_callback(): Exception: " << e.what() << std::endl;
    }
}

glm::vec3 MCRFT::Camera::getraydirection()
{
    return glm::normalize(m_camera_front);
}