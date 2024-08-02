#include "player.hpp"

void MCRFT::Player::init()
{
    m_current_pos = glm::vec3(0.0f, 98.0f, 0.0f);
    m_acceleration = glm::vec3(0.0f, -9.8f, 0.0f);
}

glm::vec3 MCRFT::Player::update_position()
{
}