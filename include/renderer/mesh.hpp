#ifndef MCRFT_MESH_HPP
#define MCRFT_MESH_HPP

#include <vector>
#include <string>
#include "renderer/texture.hpp"
#include "renderer/vertex.hpp"
#include "renderer/shader.hpp"

namespace MCRFT {
    class Mesh {
    public:
        std::vector<MCRFT::Vertex>       vertices;
        std::vector<unsigned int> indices;
        std::vector<MCRFT::Texture>      textures;

        Mesh(std::vector<MCRFT::Vertex> vertices, std::vector<unsigned int> indices, std::vector<MCRFT::Texture> textures);
        void draw(MCRFT::Shader &shader);
    private:
        unsigned int VAO, VBO, EBO;
        void setup_mesh();
    }; 
}
#endif