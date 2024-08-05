#include "renderer/texture.hpp"

#define STB_IMAGE_IMPLEMENTATION // use of stb functions once and for all
#include "external/stb_image.h"
MCRFT::TextureManager::TextureManager()
{
    m_next_texture_id = 1;
}
MCRFT::Texture::Texture(unsigned int *id)
{
    m_texture_id = *id;
}
unsigned int MCRFT::TextureManager::add_texture(std::string file_path)
{
    unsigned int res = 0;
    try
    {
        MCRFT::Texture *new_texture = new MCRFT::Texture(&m_next_texture_id);
        glGenTextures(1, &(new_texture->m_texture_id));
        glBindTexture(GL_TEXTURE_2D, new_texture->m_texture_id);
        m_texture_maps.insert({m_next_texture_id, new_texture});
        res = m_next_texture_id;
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // load image, create texture and generate mipmaps
        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
        unsigned char *data = stbi_load(std::filesystem::path(file_path).c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);
        m_next_texture_id += 1;
    }
    catch (const std::exception &e)
    {
        std::cout << "TextureManager add_texture(): Exception: " << e.what() << std::endl;
    }
    return res;
}

MCRFT::Texture *MCRFT::TextureManager::get_texture(unsigned int id)
{
    try
    {
        auto it = m_texture_maps.find(id);
        if (it != m_texture_maps.end())
        {
            return it->second;
        }
        else
        {
            return nullptr;
        }
    }
    catch (const std::exception &e)
    {
        std::cout << "TextureManager get_texture(): Exception: " << e.what() << std::endl;
    }
    return nullptr;
}