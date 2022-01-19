#pragma once
#include <iostream>

#include "Shader.h"
#include "Model.h"
#include "PerlinNoise.h"

using namespace std;

class Terrain {
	unsigned resolution;
	unsigned scale;
	float maxHeight;
	float smoothness;
	string texPath;
	PerlinNoise* perlin;
	float yOffset = 0.0f;
	float yFactor = 1.0f;
public:
	Terrain(unsigned resolution, float scale, string texPath);
	Mesh generateTerrain(float maxHeight, float smoothness, unsigned seed);
	float getHeight(float x, float z);
private:
	float getPerlinHeight(int j, int i);
	float calculateHeight(float x, float y);
};

