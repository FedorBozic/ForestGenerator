#include "Mesh.h"
#include <glm/ext/matrix_transform.hpp>

using namespace std;

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    setupMesh();
}

void Mesh::Draw(Shader& shader, DrawingMode drawingMode)
{
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;

    for (unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        std::string number;
        std::string name = textures[i].type;
        if (name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if (name == "texture_specular")
            number = std::to_string(specularNr++);

        shader.setFloat(("material." + name + number).c_str(), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    glActiveTexture(GL_TEXTURE0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    //glFrontFace(GL_CCW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::Translate(float xCoord, float yCoord, float zCoord)
{
    for (unsigned i = 0; i < vertices.size(); i++)
    {
        glm::vec4 vec(vertices[i].Position.x, vertices[i].Position.y, vertices[i].Position.z, 1.0f);
        glm::mat4 translationMatrix = glm::mat4(1.0f);
        translationMatrix = glm::translate(translationMatrix, glm::vec3(xCoord, yCoord, zCoord));
        vec = translationMatrix * vec;

        vertices[i].Position.x = vec[0];
        vertices[i].Position.y = vec[1];
        vertices[i].Position.z = vec[2];
    }
    setupMesh();
}

void Mesh::Scale(float scale)
{
    for (unsigned i = 0; i < vertices.size(); i++)
    {
        glm::vec4 vec(vertices[i].Position.x, vertices[i].Position.y, vertices[i].Position.z, 1.0f);
        glm::mat4 scaleMatrix = glm::mat4(1.0f);
        scaleMatrix = glm::scale(scaleMatrix, glm::vec3(scale, scale, scale));
        vec = scaleMatrix * vec;

        vertices[i].Position.x = vec[0];
        vertices[i].Position.y = vec[1];
        vertices[i].Position.z = vec[2];
    }
    setupMesh();
}

void Mesh::setupMesh()
{
    calculateNormals();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
        &indices[0], GL_STATIC_DRAW);

    // Vertices
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // Normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    // Texture
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
}

glm::vec3 computeFaceNormal(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
    // Uses p2 as a new origin for p1,p3
    //auto a = p3 - p2;
    //auto b = p1 - p2;

    auto a = p2 - p1;
    auto b = p3 - p1;
    // Compute the cross product a X b to get the face normal
    return glm::normalize(glm::cross(a, b));
}

void Mesh::calculateNormals() {
    //vector<glm::vec3> normals = std::vector<glm::vec3>(this->vertices.size());
    // For each face calculate normals and append it
    // to the corresponding vertices of the face
    for (unsigned i = 0; i < this->vertices.size(); i++) {
        this->vertices[i].Normal.x = 0;
        this->vertices[i].Normal.y = 0;
        this->vertices[i].Normal.z = 0;
    }
    for (unsigned int i = 0; i < this->indices.size(); i += 3) {
        glm::vec3 A = this->vertices[this->indices[i]].Position;
        glm::vec3 B = this->vertices[this->indices[i + 1]].Position;
        glm::vec3 C = this->vertices[this->indices[i + 2]].Position;
        glm::vec3 normal = computeFaceNormal(A, B, C);
        this->vertices[this->indices[i]].Normal += normal;
        this->vertices[this->indices[i + 1LL]].Normal += normal;
        this->vertices[this->indices[i + 2LL]].Normal += normal;
    }
    // Normalize each normal
    for (unsigned int i = 0; i < this->vertices.size(); i++)
        this->vertices[i].Normal = glm::normalize(this->vertices[i].Normal);

    /*for (unsigned i = 0; i < this->vertices.size(); i++) {
        this->vertices[i].Normal.x = 0.1;
        this->vertices[i].Normal.y = 0.1;
        this->vertices[i].Normal.z = 0.1;
    }*/
}