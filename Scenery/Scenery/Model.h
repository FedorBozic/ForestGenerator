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

unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);

class Model
{
    public:
        Model(const Model& modelToCopy);
        Model(char* path);
        Model(Mesh meshes);
        std::vector<Texture> textures_loaded;

        void Draw(Shader& shader, DrawingMode drawingMode = DRAWING_MODE_STANDARD);
        void RemoveSurfacePlanes();

        void Translate(float xCoord, float yCoord, float zCoord);
        void Scale(float scale);
        void Scale(float xScale, float yScale, float zScale);
        void Rotate(float xRot, float yRot, float zRot, float angle);

        Vertex GetRandomVertex();
    private:
        std::vector<Mesh> meshes;
        std::string directory;
    
        void loadModel(std::string path);
        void processNode(aiNode* node, const aiScene* scene);
        Mesh processMesh(aiMesh* mesh, const aiScene* scene);
        std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};

#endif