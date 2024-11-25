#ifndef MCRFT_TEXTURE_HPP
#define MCRFT_TEXTURE_HPP

#include <filesystem>
#include <iostream>
#include <string>
#include <unordered_map>

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace MCRFT
{
class Texture
{
public:
    Texture();
    Texture(unsigned int *id);
    unsigned int m_texture_id;
    std::string m_type;
    std::string m_path;

private:
};

class TextureManager
{
public:
    TextureManager();
    unsigned int addtexture(std::string file_path);
    MCRFT::Texture* gettexture(unsigned int id);

private:
    unsigned int m_next_texture_id;
    std::unordered_map<unsigned int, MCRFT::Texture *> m_texture_maps;
};
}
#endif