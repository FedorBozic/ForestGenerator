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

#define SCR_WIDTH 800
#define SCR_HEIGHT 600

#define ROTATION_RADIUS 20.0f

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
"layout(location = 1) in vec2 aTexCoord;\n"
"out vec2 TexCoord;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"    gl_Position = projection * view * model * vec4(aPos, 1.0f);\n"
"    TexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);\n"
"};\n";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\0";

Mesh generateTerrain(int size) {
    float scale = 10;
    float step = scale / size;

    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            Vertex newVertex;
            newVertex.Position.x = j * step;
            newVertex.Position.y = rand() % 10 * step / 4;
            newVertex.Position.z = i * step;
            vertices.push_back(newVertex);
        }

        if (i > 0) {
            int t1, t2, t3, t4;
            for (int j = 0; j < size - 1; j++) {
                t1 = ((i - 1) * size + j);
                t2 = ((i - 1) * size + (j + 1));
                t3 = (i * size + j);
                t4 = (i * size + (j + 1));

                cout << "[" << t1 << ", " << t2 << ", " << t3 << "], ";
                cout << "[" << t2 << ", " << t3 << ", " << t4 << "]" << endl;

                indices.push_back(t1);
                indices.push_back(t2);
                indices.push_back(t3);
                indices.push_back(t2);
                indices.push_back(t3);
                indices.push_back(t4);
            }
        }
    }

    Texture blankTexture;
    blankTexture.id = 0;
    blankTexture.type = "NO_TYPE";
    blankTexture.path = "NO_PATH";
    textures.push_back(blankTexture);

    Mesh terrain(vertices, indices, textures);
    return terrain;
}

unsigned initShader() {
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    //Optional check success

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

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

    Shader shader(vertexShaderSource, fragmentShaderSource);
    glViewport(0, 0, 800, 600);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    Mesh terrain = generateTerrain(20);
    Model surfaceModel(terrain);
    surfaceModel.Translate(-5.0f, 0.0f, -5.0f);

    string str_obj = "C:/Users/fedor/OneDrive/Desktop/RG/scenery/Scenery/resources/tree/Tree.obj";
    Model treeModel(&str_obj[0]);
    vector<Model> treeModels = getScatteredModelsAcrossSurface(surfaceModel, treeModel, 5);

    string rock_obj_path = "C:/Users/fedor/OneDrive/Desktop/RG/scenery/Scenery/resources/rock1/Rock1.obj";
    Model rockModel(&rock_obj_path[0]);
    rockModel.RemoveSurfacePlanes();
    rockModel.Scale(0.2f);
    vector<Model> rockModels = getScatteredModelsAcrossSurface(surfaceModel, rockModel, 10);

    unsigned shaderProgram = initShader();

    shader.use();

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

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
        surfaceModel.Draw(shader);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);

        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        float camX = sin(glfwGetTime()) * ROTATION_RADIUS;
        float camZ = cos(glfwGetTime()) * ROTATION_RADIUS;
        view = glm::lookAt(glm::vec3(camX, 3.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

        shader.setMat4("model", model);
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}