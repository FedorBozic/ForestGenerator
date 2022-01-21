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
#include "Possion.h"
#include <stb/stb_image.h>
#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imgui_impl_opengl3.h"

#define SCR_WIDTH 1024
#define SCR_HEIGHT 576

#define ROTATION_RADIUS 50.0f
#define ROTATION_SPEED 0.5f
#define CAMERA_HEIGHT 30.0f

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

const char* sunSurfaceVertexShader = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"}\0";

const char* sunSurfaceFragmentShader = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"    FragColor = vec4(1.0);\n"
"}\0";

const unsigned seed = 501;
PerlinNoise perlin(seed);
const float scale = 40;
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

vector<Model> getTreeModelsFromPositions(Terrain terrain, Model objectTemplate, list<Point> treePositions)
{
    vector<Model> result;
    std::list<Point>::iterator it;
    it = treePositions.begin();
    
    for (unsigned int i = 0; i < treePositions.size(); i++)
    {
        Point point = *it;

        Model* newModel = new Model(objectTemplate);
        newModel->Translate(point.x, terrain.getHeight(point.x, point.y), point.y);
        result.push_back(*newModel);
        advance(it, 1);
    }
    return result;
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
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

    Shader shader("shader.vs", "shader.fs");
    Shader sunSurfaceShader("sunShader.vs", "sunShader.fs");
    Shader shaderPassthrough("shaderPassthrough.vs", "shaderPassthrough.fs");
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

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

    string currentUser = fedor;

    string tree_path = currentUser + "scenery/Scenery/resources/tree1/tree1.obj";
    Model treeModel(&tree_path[0]);

    string flower_pink_path = currentUser + "scenery/Scenery/resources/flower1/flower1.obj";
    Model flowerModelPink(&flower_pink_path[0]);
    flowerModelPink.Scale(0.1f);
    string flower_white_path = currentUser + "scenery/Scenery/resources/flower2/flower1.obj";
    Model flowerModelWhite(&flower_white_path[0]);
    flowerModelWhite.Scale(0.1f);
    string grass_path = currentUser + "scenery/Scenery/resources/grass/grass.obj";
    Model grassModel(&grass_path[0]);

    string grassTex = currentUser + "scenery/Scenery/resources/tex_grass.png";
    Terrain terrain(perlinResolution, scale, grassTex);
    Mesh terrainMesh = terrain.generateTerrain(maxHeight, smoothness, seed);
    Model surfaceModel(terrainMesh);
    surfaceModel.Translate(0.0f, 0.0f, 0.0f);

    string str_sun_path = currentUser + "scenery/Scenery/resources/sun/sun.obj";
    Model sunModel(&str_sun_path[0]);

    //vector<Model> treeModels = getScatteredModelsAcrossSurface(surfaceModel, treeModel, 5);
    Possion possion;
    list<Point> treePositions = possion.GeneratePossion(scale, 5.0, 5.0, 30);
    vector<Model> treeModels = getTreeModelsFromPositions(terrain, treeModel, treePositions);
    list<Point> flowerPinkPositions = possion.GeneratePossion(scale, 5.0, 5.0, 30);
    vector<Model> flowerPinkModels = getTreeModelsFromPositions(terrain, flowerModelPink, flowerPinkPositions);
    list<Point> flowerWhitePositions = possion.GeneratePossion(scale, 5.0, 5.0, 30);
    vector<Model> flowerWhiteModels = getTreeModelsFromPositions(terrain, flowerModelWhite, flowerWhitePositions);
    list<Point> grassPositions = possion.GeneratePossion(scale, 5.0, 5.0, 30);
    vector<Model> grassModels = getTreeModelsFromPositions(terrain, grassModel, grassPositions);

    string str_sky = currentUser + "scenery/Scenery/resources/sky/sky.obj";
    Model skyModel(&str_sky[0]);
    skyModel.Scale(1000.0f);
    skyModel.Translate(10, 6, 10);

    string rock_obj_path = currentUser + "scenery/Scenery/resources/rock1/Rock1.obj";
    Model rockModel(&rock_obj_path[0]);
    Model lightModel(&rock_obj_path[0]);
    rockModel.RemoveSurfacePlanes();
    lightModel.RemoveSurfacePlanes();
    rockModel.Scale(0.2f);
    vector<Model> rockModels = getScatteredModelsAcrossSurface(surfaceModel, rockModel, 10);

    /*string str_backpack_path = currentUser + "scenery/Scenery/resources/test_backpack/backpack.obj";
    Model backpackModel(&str_backpack_path[0]);
    backpackModel.Scale(5.0f);
    backpackModel.Translate(10.0f, 0.0f, 10.0f);*/

    //poisson_distribution //postoji??

    float lightPosition[3] = { 0.0f, 0.0f, 0.0f };

    /* IMGUI SETUP */
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
    ImGui::StyleColorsDark();

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);

        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);

        float camX = scale / 2 + sin(ROTATION_SPEED * glfwGetTime()) * ROTATION_RADIUS;
        float camZ = scale / 2 + cos(ROTATION_SPEED * glfwGetTime()) * ROTATION_RADIUS;
        float camY = CAMERA_HEIGHT;
        view = glm::lookAt(glm::vec3(camX, camY, camZ), glm::vec3(scale / 2, 0.0, scale / 2), glm::vec3(0.0, 1.0, 0.0));

        shaderPassthrough.use();
        skyModel.Draw(shaderPassthrough);

        ImGui::Begin("Light Settings");
        ImGui::SliderFloat3("position", lightPosition, 0.0, 20.0);
        static float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        ImGui::ColorEdit3("color", color);
        ImGui::End();

        shaderPassthrough.setVec3("lightColor", color[0], color[1], color[2]);
        shaderPassthrough.setVec3("lightPos", lightPosition[0], lightPosition[1], lightPosition[2]);
        shaderPassthrough.setVec3("viewPos", camX, camZ, camY);
        shaderPassthrough.setMat4("model", model);
        shaderPassthrough.setMat4("projection", projection);
        shaderPassthrough.setMat4("view", view);

        shader.use();
        surfaceModel.Draw(shader);
        treeModel.Draw(shader);
        for (int i = 0; i < treeModels.size(); i++)
        {
            treeModels[i].Draw(shader);
        }
        for (int i = 0; i < flowerPinkModels.size(); i++)
        {
            flowerPinkModels[i].Draw(shader);
        }
        for (int i = 0; i < flowerWhiteModels.size(); i++)
        {
            flowerWhiteModels[i].Draw(shader);
        }
        for (int i = 0; i < grassModels.size(); i++)
        {
            grassModels[i].Draw(shader);
        }
        for (int i = 0; i < rockModels.size(); i++)
        {
            rockModels[i].Draw(shader);
        }
        
        //backpackModel.Draw(shader);

        shader.setVec3("lightColor", color[0], color[1], color[2]);
        shader.setVec3("lightPos", lightPosition[0], lightPosition[1], lightPosition[2]);
        shader.setVec3("viewPos", camX, camZ, camY);
        shader.setMat4("model", model);
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        sunSurfaceShader.use();
        sunSurfaceShader.setVec3("lightColor", color[0], color[1], color[2]);
        sunSurfaceShader.setFloat("threshold", 0.5);
        sunSurfaceShader.setMat4("projection", projection);
        sunSurfaceShader.setMat4("view", view);
        sunSurfaceShader.setMat4("model", model);

        Model lightModelTranslated = Model(sunModel);
        lightModelTranslated.Translate(lightPosition[0], lightPosition[1], lightPosition[2]);
        lightModelTranslated.Draw(sunSurfaceShader);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}