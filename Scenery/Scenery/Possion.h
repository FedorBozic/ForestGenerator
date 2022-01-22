#pragma once
#include <vector>
#include <list>

#include "PerlinNoise.h"

using namespace std;

struct Point {
	float x = -1;
	float y = -1;
};

class Possion
{
	private:
		unsigned scale;
		float smoothness;
		float treshold;
		PerlinNoise* perlin;

		float calculatePerlinValue(float x, float y);
	public:
		Possion(unsigned scale, float smoothness, float treshold);
		list<Point> generatePossion(float size, float minDist1, float minDist2, int newPointsCount, unsigned seed);
};

