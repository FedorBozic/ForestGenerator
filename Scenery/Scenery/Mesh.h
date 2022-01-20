#ifndef MESH_H
#define MESH_H

// OpenGL includes
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float2.hpp>

// Project includes
#include "Shader.h"

// Standard includes
#include <string>
#include <vector>

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

enum DrawingMode {
    DRAWING_MODE_STANDARD,
    DRAWING_MODE_WIREFRAME
};

class Mesh
{
    public:
        std::vector<Vertex>       vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture>      textures;

        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

        void Draw(Shader& shader, DrawingMode drawingMode = DRAWING_MODE_STANDARD);

        void Translate(float xCoord, float yCoord, float zCoord);
        void Scale(float scale);
    private:
        unsigned int VAO, VBO, EBO;

        void setupMesh();
        void calculateNormals();
};

#endif