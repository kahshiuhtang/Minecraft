#ifndef MCRFT_MODEL_HPP
#define MCRFT_MODEL_HPP

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.h"
#include <assimp/Importer.hpp> 
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "renderer/shader.hpp"
#include "renderer/mesh.hpp"

#include <string>
#include <vector>
namespace MCRFT {
class Model 
{
    public:
    
        Model(const char *path)
        {
            load_model(path);
        }
        void draw(MCRFT::Shader &shader){
            for(unsigned int i = 0; i < meshes.size(); i++)
                meshes[i].draw(shader);
        }	
    private:
        std::vector<MCRFT::Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
        std::vector<MCRFT::Mesh>    meshes;
        std::string directory;
        bool gammaCorrection;

        void load_model(std::string path);
        void process_node(aiNode *node, const aiScene *scene);
        Mesh process_mesh(aiMesh *mesh, const aiScene *scene);
        std::vector<Texture> load_material_textures(aiMaterial *mat, aiTextureType type, 
                                             std::string typeName);
};
}
#endif