#include<iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define _USE_MATH_DEFINES
#include <math.h>

#include <iostream>
#include <cstring>

#include "Shader.h"
#include "Model.h"
#include "PerlinNoise.h"
#include "Terrain.h"
#include <stb/stb_image.h>

#define SCR_WIDTH 800
#define SCR_HEIGHT 600

#define ROTATION_RADIUS 20.0f
#define ROTATION_SPEED 0.5f

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

const char* vertexShaderSource = "#version 330 core\n"
"layout(location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aNormal;"
"layout(location = 2) in vec2 aTexCoords;\n"
"out vec2 TexCoords;\n"
"out vec3 Normal;\n"
"out vec3 Position;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"    gl_Position = projection * view * vec4(aPos, 1.0f);\n"
"    TexCoords = aTexCoords;\n"
"    Normal = aNormal;\n"
"    Normal = mat3(transpose(inverse(model))) * aNormal;\n"
"    //Position = aPos;\n"
"    Position = vec3(model * vec4(aPos, 1.0));\n"
"    //Normal = mat3(transpose(inverse(model))) * aNormal\n"
"    //gl_Position = projection * view * vec4(aPos, 1.0);\n"
"};\n";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 Position;\n"
"in vec3 Normal;\n"
"in vec2 TexCoords;\n"
"uniform sampler2D texture_diffuse1;\n"
"uniform vec3 lightPos;\n"
"uniform vec3 viewPos;\n"
"uniform vec3 lightColor;\n"
"void main()\n"
"{\n"
"   float ambientStrength = 0.2;\n"
"   vec3 ambient = ambientStrength * lightColor;\n"
"   vec3 norm = normalize(Normal);\n"
"   vec3 lightDir = normalize(lightPos - Position);\n"
"   float diff = max(dot(norm, lightDir), 0.0);\n"
"   vec3 diffuse = diff * lightColor;\n"
"   vec4 texCol = texture(texture_diffuse1, TexCoords);\n"
"   vec3 result = (ambient + diffuse) * texCol.rgb;\n"
"   FragColor = vec4(result, texCol.a);\n"
"}\0";

const unsigned seed = 501;
PerlinNoise perlin(seed);
const float scale = 20;
const float smoothness = 10;
const int perlinResolution = 32; //256, 32
const float maxHeight = 4;

vector<Model> getScatteredModelsAcrossSurface(Model surface, Model objectTemplate, unsigned int count)
{
    vector<Model> result;
    for (unsigned int i = 0; i < count; i++)
    {
        Vertex randomVertex = surface.GetRandomVertex();

        Model* newModel = new Model(objectTemplate);
        newModel->Translate(randomVertex.Position.x, randomVertex.Position.y, randomVertex.Position.z);
        result.push_back(*newModel);
    }
    return result;
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //Shader shader(vertexShaderSource, fragmentShaderSource);
    Shader shader("shader.vs", "shader.fs");
    glViewport(0, 0, 800, 600);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0.0f, 1.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    string igor = "C:/Users/SI/Documents/GitHub/";
    string fedor = "C:/Users/fedor/OneDrive/Desktop/RG/";

    string currentUser = igor;

    string grassTex = currentUser + "scenery/Scenery/resources/tex_grass.png";
    Terrain terrain(perlinResolution, scale, grassTex);
    Mesh terrainMesh = terrain.generateTerrain(maxHeight, smoothness, seed);
    Model surfaceModel(terrainMesh);
    surfaceModel.Translate(0.0f, 0.0f, 0.0f);

    string str_obj = currentUser + "scenery/Scenery/resources/tree/Tree.obj";
    Model treeModel(&str_obj[0]);
    vector<Model> treeModels = getScatteredModelsAcrossSurface(surfaceModel, treeModel, 5);

    string rock_obj_path = currentUser + "scenery/Scenery/resources/rock1/Rock1.obj";
    Model rockModel(&rock_obj_path[0]);
    rockModel.RemoveSurfacePlanes();
    rockModel.Scale(0.2f);
    vector<Model> rockModels = getScatteredModelsAcrossSurface(surfaceModel, rockModel, 10);

    shader.use();

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        surfaceModel.Draw(shader);
        treeModel.Draw(shader);
        for (int i = 0; i < treeModels.size(); i++)
        {
            treeModels[i].Draw(shader);
        }
        rockModel.Draw(shader);
        for (int i = 0; i < rockModels.size(); i++)
        {
            rockModels[i].Draw(shader);
        }

        shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        shader.setVec3("lightPos", 10.0f, 10.0f, 10.0f);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);

        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        float camX = scale / 2 + sin(ROTATION_SPEED * glfwGetTime()) * ROTATION_RADIUS;
        float camZ = scale / 2 + cos(ROTATION_SPEED * glfwGetTime()) * ROTATION_RADIUS;
        float camY = 10.0f;
        view = glm::lookAt(glm::vec3(camX, camY, camZ), glm::vec3(scale / 2, 0.0, scale / 2), glm::vec3(0.0, 1.0, 0.0));

        shader.setMat4("model", model);
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}