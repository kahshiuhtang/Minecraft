#ifndef MCRFT_PLAYER_HPP
#define MCRFT_PLAYER_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace MCRFT
{
    class Player
    {
    public:
        Player();
        glm::vec3 update_position();

        glm::vec3 m_current_pos;
        glm::vec3 m_velocity;
        glm::vec3 m_acceleration;

    private:
    };
}
#endif