#pragma once
#include <vector>
#include <list>

using namespace std;

struct Point {
	float x = -1;
	float y = -1;
};

class Possion
{
	public:
		Possion();
		list<Point> GeneratePossion(float size, float minDist1, float minDist2, int newPointsCount);
};

