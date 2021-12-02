#ifndef MODEL_H
#define MODEL_H

// OpenGL includes
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float2.hpp>

// Assimp includes
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Project includes
#include "Shader.h"
#include "Mesh.h"

// Standard includes
#include <string>
#include <vector>

class Model
{
    public:
        Model(char* path);

        void Draw(Shader& shader);
    private:
        std::vector<Mesh> meshes;
        std::string directory;
    
        void loadModel(std::string path);
        void processNode(aiNode* node, const aiScene* scene);
        Mesh processMesh(aiMesh* mesh, const aiScene* scene);
        std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};

#endif