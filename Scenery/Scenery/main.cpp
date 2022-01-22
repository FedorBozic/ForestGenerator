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

#define SHADOW_WIDTH 1024
#define SHADOW_HEIGHT 1024

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

unsigned seed = 501;
PerlinNoise perlin(seed);
float scale = 40;
float smoothness = 10;
const int perlinResolution = 32; //256, 32
float maxHeight = 4;

vector<Model> treeModels;
vector<Model> rockModels;
vector<Model> flowerModels;

static float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

float camX = 0;
float camY = 0;
float camZ = 10;

float lightPosition[3] = { 0.0f, 0.0f, 0.0f };

const float daySpeed = 0.2;
glm::vec3 lightDir(0.0);
float lightStrength = 0;
float dayTime = 0;

int getRandomSeed()
{
    return 500 + rand() % 10000;
}

vector<Model> getScatteredModelsAcrossSurface(Model surface, Model objectTemplate, unsigned int count)
{
    vector<Model> result;
    for (unsigned int i = 0; i < count; i++)
    {
        Vertex randomVertex = surface.GetRandomVertex();

        Model newModel(objectTemplate);
        newModel.Translate(randomVertex.Position.x, randomVertex.Position.y, randomVertex.Position.z);
        result.push_back(newModel);
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

        //Model* newModel = new Model(objectTemplate);
        Model newModel(objectTemplate);
        newModel.Scale(0.8 + (float)(rand() % 10000) / 10000 * 0.4);
        newModel.Rotate(0, 1, 0, ((float)(rand() % 10000)) / 10000 * M_PI * 2);
        newModel.Translate(point.x, terrain.getHeight(point.x, point.y), point.y);
        result.push_back(newModel);
        advance(it, 1);
    }
    return result;
}

vector<Model> scatterModelsAcrossTerrain(Terrain terrain, vector<Model> templateModels)
{
    Possion possion(scale, 10, 0);
    vector<Model> allScatteredModels;
    for (int i = 0; i < templateModels.size(); i++)
    {
        list<Point> modelPositions = possion.generatePossion(scale, 1.5, 1.5, 30, getRandomSeed());
        vector<Model> tmpModelList = getTreeModelsFromPositions(terrain, templateModels[i], modelPositions);
        for (int j = 0; j < tmpModelList.size(); j++)
        {
            allScatteredModels.push_back(tmpModelList[j]);
        }
    }
    return allScatteredModels;
}

void progressDay()
{
    float angle = (daySpeed * glfwGetTime());
    lightDir.x = cos(angle);
    lightDir.y = sin(angle);

    if (lightDir.y < 0.1 && lightDir.y >= -0.2) {
        lightStrength = 10 / 3 * lightDir.y + 2 / 3;
    }
    else if (lightDir.y < -0.2) {
        lightStrength = 0;
    }
    else lightStrength = 1;
    //lightDir.x = 1;
    //lightDir.y = 1;
    //lightDir.z = 1;

    
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
    //Shader depthShader("depthShader.vs", "depthShader.fs");
    //Shader shadowmapShader("shadowmapShader.vs", "shadowmapShader.fs");
    Shader sunSurfaceShader("sunShader.vs", "sunShader.fs");
    Shader shaderPassthrough("shaderPassthrough.vs", "shaderPassthrough.fs");
    //Shader debugDepthQuad("debugDepthQuad.vs", "debugDepthQuad.fs");
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

    string currentUser = igor;

    string grassTex = currentUser + "scenery/Scenery/resources/tex_grass.png";
    Terrain terrain(perlinResolution, scale, grassTex);
    Mesh terrainMesh = terrain.generateTerrain(maxHeight, smoothness, seed);
    Model surfaceModel(terrainMesh);
    surfaceModel.Translate(0.0f, 0.0f, 0.0f);

    string str_sun_path = currentUser + "scenery/Scenery/resources/sun/sun.obj";
    Model sunModel(&str_sun_path[0]);

    string grass_path = currentUser + "scenery/Scenery/resources/grass/grass.obj";
    Model grassModel(&grass_path[0]);
    //grassModel.Scale(1.0, -1.0, 1.0);

    //string tree_path = currentUser + "scenery/Scenery/resources/tree1/tree1.obj";
    string tree_path = currentUser + "scenery/Scenery/resources/tree/Tree.obj";
    Model treeModel(&tree_path[0]);
    //vector<Model> treeModels = getScatteredModelsAcrossSurface(surfaceModel, treeModel, 5);
    Possion possion(scale, 10, 0.55);
    list<Point> treePositions = possion.generatePossion(scale, 2, 2, 30, getRandomSeed());
    vector<Model> treeModels = getTreeModelsFromPositions(terrain, treeModel, treePositions);

    string flower_pink_path = currentUser + "scenery/Scenery/resources/flower1/flower1.obj";
    Model flowerModelPink(&flower_pink_path[0]);
    flowerModelPink.Scale(0.1f);
    Possion flowerPossion(scale, 10, 0);
    list<Point> flowerPositions = flowerPossion.generatePossion(scale, 7, 7, 30, getRandomSeed());
    flowerModels = getTreeModelsFromPositions(terrain, flowerModelPink, flowerPositions);

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

    vector<Model> allScatterTemplateModels;
    //allScatterTemplateModels.push_back(treeModel);
    //allScatterTemplateModels.push_back(flowerModelPink);
    allScatterTemplateModels.push_back(grassModel);
    vector<Model> allScatteredModels = scatterModelsAcrossTerrain(terrain, allScatterTemplateModels);

    Model lightModelTranslated = Model(sunModel);

    float lightPosition[3] = { 0.0f, 0.0f, 0.0f };
    float changedScale = scale;
    float changedSmoothness = smoothness;
    float changedMaxHeight = maxHeight;

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

        //glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Zasto dva puta?

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);

        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);

        camX = scale / 2 + sin(ROTATION_SPEED * glfwGetTime()) * ROTATION_RADIUS;
        camZ = scale / 2 + cos(ROTATION_SPEED * glfwGetTime()) * ROTATION_RADIUS;
        camY = CAMERA_HEIGHT;
        view = glm::lookAt(glm::vec3(camX, camY, camZ), glm::vec3(scale / 2, 0.0, scale / 2), glm::vec3(0.0, 1.0, 0.0));

        progressDay();

        ImGui::Begin("Light Settings");
        ImGui::SliderFloat3("position", lightPosition, 0.0, scale);
        ImGui::ColorEdit3("color", color);
        ImGui::End();

        ImGui::Begin("Terrain Settings");
        ImGui::SliderFloat("Scale", &changedScale, 0.0, 50.0);
        ImGui::SliderFloat("Smoothness", &changedSmoothness, 0.0, 20.0);
        ImGui::SliderFloat("Max Height", &changedMaxHeight, 0.0, 10.0);
        if (ImGui::Button("Regenerate Terrain"))
        {
            seed = getRandomSeed();
            scale = changedScale;
            smoothness = changedSmoothness;
            maxHeight = changedMaxHeight;
            Terrain newTerrain(perlinResolution, scale, grassTex);
            Mesh newTerrainMesh = newTerrain.generateTerrain(maxHeight, smoothness, seed);
            Model newSurfaceModel(newTerrainMesh);
            newSurfaceModel.Translate(0.0f, 0.0f, 0.0f);
            terrain = newTerrain;
            surfaceModel = newSurfaceModel;
            allScatteredModels = scatterModelsAcrossTerrain(terrain, allScatterTemplateModels);

            treePositions = possion.generatePossion(scale, 2, 2, 30, getRandomSeed());
            treeModels = getTreeModelsFromPositions(terrain, treeModel, treePositions);

            list<Point> rockPositions = possion.generatePossion(scale, 10, 10, 30, getRandomSeed());
            rockModels = getTreeModelsFromPositions(terrain, rockModel, rockPositions);

            flowerPositions = flowerPossion.generatePossion(scale, 10, 10, 30, getRandomSeed());
            flowerModels = getTreeModelsFromPositions(terrain, flowerModelPink, flowerPositions);
        }
        ImGui::End();
        
        shaderPassthrough.use();
        skyModel.Draw(shaderPassthrough);

        shaderPassthrough.setVec3("lightColor", color[0], color[1], color[2]);
        shaderPassthrough.setVec3("lightPos", lightPosition[0], lightPosition[1], lightPosition[2]);
        shaderPassthrough.setFloat("lightStrength", lightStrength);
        shaderPassthrough.setVec3("viewPos", camX, camZ, camY);
        shaderPassthrough.setMat4("model", model);
        shaderPassthrough.setMat4("projection", projection);
        shaderPassthrough.setMat4("view", view);

        
        shader.use();
        surfaceModel.Draw(shader);
        //treeModel.Draw(shader);
        for (int i = 0; i < treeModels.size(); i++)
        {
            treeModels[i].Draw(shader);
        }
        for (int i = 0; i < rockModels.size(); i++)
        {
            rockModels[i].Draw(shader);
        }

        shader.setVec3("lightColor", color[0], color[1], color[2]);
        shader.setVec3("lightPos", lightPosition[0], lightPosition[1], lightPosition[2]);
        shader.setVec3("lightDir", lightDir);
        shader.setVec3("viewPos", camX, camZ, camY);
        shader.setMat4("model", model);
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        shaderPassthrough.use();
        for (int i = 0; i < allScatteredModels.size(); i++)
        {
            allScatteredModels[i].Draw(shader);
        }
        for (int i = 0; i < flowerModels.size(); i++)
        {
            flowerModels[i].Draw(shader);
        }

        sunSurfaceShader.use();
        sunSurfaceShader.setVec3("lightColor", color[0], color[1], color[2]);
        sunSurfaceShader.setFloat("threshold", 0.5);
        sunSurfaceShader.setMat4("projection", projection);
        sunSurfaceShader.setMat4("view", view);
        sunSurfaceShader.setMat4("model", model);

        
        //lightModelTranslated.Translate(lightPosition[0], lightPosition[1], lightPosition[2]);
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