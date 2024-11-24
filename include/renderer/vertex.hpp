#ifndef MCRFT_VERTEX_HPP
#define MCRFT_VERTEX_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace MCRFT {
    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
    };
}
#endif