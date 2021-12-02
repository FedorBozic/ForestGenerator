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

struct Object {
    vector<float> vertices;
    vector<unsigned> indices;
};

struct Point {
    float x;
    float y;
    float z;
};

void setupHalfShpere(Object& sphere, float r, int detail, bool top) {
    float pi = M_PI;
    float step = 2 * pi / detail;
    int dir = 1;
    int startPoint = 0;
    if (!top) {
        dir = -1;
        startPoint = sphere.vertices.size() / 3;
    }
    for (int i = 0; i < ceil(((float)detail) / 2) - 1; i++) {
        if (!top && i == 0)
            continue;
        for (int j = 0; j < detail; j++) {

            float pitch = i * step;
            float tetha = j * step;

            float x = cos(tetha) * r * cos(pitch);
            float y = sin(tetha) * r * cos(pitch);
            float z = sin(pitch) * r * dir;

            sphere.vertices.push_back(x);
            sphere.vertices.push_back(y);
            sphere.vertices.push_back(z);
        }
        if (i > 0) {
            int t1, t2, t3, t4;
            for (int j = 0; j < detail; j++) {
                t1 = startPoint + ((i - 1) * detail + j);
                t2 = startPoint + ((i - 1) * detail + ((j + 1) % detail));
                t3 = startPoint + (i * detail + j);
                t4 = startPoint + (i * detail + ((j + 1) % detail));

                cout << "[" << t1 << ", " << t2 << ", " << t3 << "], ";
                cout << "[" << t2 << ", " << t3 << ", " << t4 << "]" << endl;

                sphere.indices.push_back(t1);
                sphere.indices.push_back(t2);
                sphere.indices.push_back(t3);
                sphere.indices.push_back(t2);
                sphere.indices.push_back(t3);
                sphere.indices.push_back(t4);
            }
        }
    }
    bool drawTop = true;
    if (drawTop) {
        sphere.vertices.push_back(0);
        sphere.vertices.push_back(0);
        sphere.vertices.push_back(r * dir);
        int topi = detail / 2 - 1;
        for (int i = 0; i < detail; i++) {
            int t1 = startPoint + ((topi - 1) * detail + i);
            int t2 = startPoint + ((topi - 1) * detail + ((i + 1) % detail));
            int t3 = sphere.vertices.size() / 3;
            sphere.indices.push_back(t1);
            sphere.indices.push_back(t2);
            sphere.indices.push_back(t3);
            cout << "[" << t1 << ", " << t2 << ", " << t3 << "]" << endl;
        }
        cout << topi << endl;
    }
}

Object createSphere(float r) {
    int detail = 10;
    //float pi = 3.1416f;
    Object sphere;

    setupHalfShpere(sphere, r, detail, true);
    //setupHalfShpere(sphere, r, detail, false);

    return sphere;
}

Object generateTerrain(int size) {
    Object terrain;
    float scale = 10;
    float step = scale / size;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            terrain.vertices.push_back(j * step);
            terrain.vertices.push_back(i * step);
            terrain.vertices.push_back(rand() % 10 * step / 4);
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

                terrain.indices.push_back(t1);
                terrain.indices.push_back(t2);
                terrain.indices.push_back(t3);
                terrain.indices.push_back(t2);
                terrain.indices.push_back(t3);
                terrain.indices.push_back(t4);
            }
        }
    }
    return terrain;
}

Object sphere;
Object terrain = generateTerrain(20);

unsigned initRender() {
    vector<float> vertices = terrain.vertices;
    vector<unsigned> indices = terrain.indices;

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    // 1. bind Vertex Array Object
    glBindVertexArray(VAO);
    // 2. copy our vertices array in a vertex buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW); //sizeof vertices
    // 3. copy our index array in a element buffer for OpenGL to use
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned), indices.data(), GL_STATIC_DRAW); //sizeof indices
    // 4. then set the vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
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

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

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

    vector<float> vertices = terrain.vertices;
    vector<unsigned> indices = terrain.indices;

    string str_obj = "C:/Users/fedor/OneDrive/Desktop/RG/scenery/Scenery/resources/tree/Tree.obj";
    Model ourModel(&str_obj[0]);

    unsigned shaderProgram = initShader();
    unsigned VAO = initRender();

    shader.use();

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        shader.use();

        glm::mat4 model = glm::mat4(1.0f);
        float angle = 20.0f;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        shader.setMat4("model", model);
        ourModel.Draw(shader);

        glBindVertexArray(VAO);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLES, indices.size() * 3, GL_UNSIGNED_INT, 0);

        glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        const float radius = 10.0f;
        float camX = sin(glfwGetTime()) * radius;
        float camZ = cos(glfwGetTime()) * radius;
        view = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

        // pass transformation matrices to the shader
        shader.setMat4("projection", projection); // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
        shader.setMat4("view", view);

        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}