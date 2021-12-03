#pragma once
#include <iostream>
#include <vector>
#include <numeric>
#include <random>

using namespace std;

class PerlinNoise {
	vector<int> permutation;
public:
	PerlinNoise();
	PerlinNoise(unsigned seed);
	double getNoise(double x, double y, double z = 0);
private:
	double fade(double t);
	double lerp(double t, double a, double b);
	double grad(int hash, double x, double y, double z = 0);
};