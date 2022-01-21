#include "Possion.h"
#include <stdlib.h>
#include <iostream>

#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

Point pop(list<Point>& list, int index) {
	std::list<Point>::iterator it;
	it = list.begin();
	advance(it, index);
	Point point = *it;
	list.erase(it);
	return point;
}

Point createPoint(float x, float y) {
	Point point;
	point.x = x;
	point.y = y;
	return point;
}

float randomRange(float x1, float x2) {
	//int precision = 10000;
	float dif = x2 - x1;
	//return (((float)(rand() % precision)) / precision) * dif - x1;
	return (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * dif + x1;
}

Point positionToGrid(Point point, float cellSize) {
	int gridX = (int)(point.x / cellSize);
	int gridY = (int)(point.y / cellSize);
	return createPoint(gridX, gridY);
}

Point generateRandomPointAround(Point point, float minDist) {
	float r1 = randomRange(1, 2);
	float r2 = randomRange(0, 1);

	float radius = minDist * r1;

	float angle = 2 * M_PI * r2;

	float newX = point.x + radius * cos(angle);
	float newY = point.y + radius * sin(angle);
	return createPoint(newX, newY);
}

bool pointInRectangle(Point point, float size) {
	return point.x >= 0 and point.y >= 0 and point.x <= size and point.y <= size;
}

vector<Point> squareAroundPoint(vector<vector<Point>>& grid, Point gridPoint, int count) {
	int fromX = max(gridPoint.x - count / 2, 0.0f);
	int toX = min(gridPoint.x + count / 2, grid.size() - 1.0f);
	int fromY = max(gridPoint.y - count / 2, 0.0f);
	int toY = min(gridPoint.y + count / 2, grid.size() - 1.0f);
	vector<Point> squares;
	for (int j = fromX; j <= toX; j++) {
		for (int i = fromY; i <= toY; i++) {
			squares.push_back(createPoint(j, i));
		}
	}
	return squares;
}

float pointDistance(Point p1, Point p2) {
	return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

bool pointInNeighbourhood(vector<vector<Point>>& grid, Point point, float minDist, float cellSize) {
	Point gridPoint = positionToGrid(point, cellSize);

	vector<Point> cellsAroundPoint = squareAroundPoint(grid, gridPoint, 5);
	for (int i = 0; i < cellsAroundPoint.size(); i++) {
		Point cell = cellsAroundPoint[i];
		Point cellContent = grid[cell.x][cell.y];
		if (cellContent.x != -1 && pointDistance(cellContent, point) < minDist) //cellContent?
			return true;
	}
	return false;
}

Possion::Possion() {
	cout << "Done" << endl;
}

list<Point> Possion::GeneratePossion(float size, float minDist1, float minDist2, int newPointsCount) {
	float cellSize = minDist1 / sqrt(2);

	int gridSize = ceil(size / cellSize);

	vector<vector<Point>> grid(gridSize);
	for (int i = 0; i < gridSize; i++) {
		grid[i].resize(gridSize);
	}

	list<Point> processList;
	list<Point> samplePoints;

	Point firstPoint = createPoint(randomRange(0, size), randomRange(0, size));

	processList.push_back(firstPoint);
	samplePoints.push_back(firstPoint);

	Point gridPoint = positionToGrid(firstPoint, cellSize);
	grid[gridPoint.x][gridPoint.y] = firstPoint;

	while (!processList.empty()) {
		Point point = pop(processList, rand() % processList.size());
		for (int i = 0; i < newPointsCount; i++) {
			Point newPoint = generateRandomPointAround(point, minDist1); //minDist1 za sad

			if (pointInRectangle(newPoint, size) and !pointInNeighbourhood(grid, newPoint, minDist1, cellSize)) {
				processList.push_back(newPoint);
				samplePoints.push_back(newPoint);
				gridPoint = positionToGrid(newPoint, cellSize);
				grid[gridPoint.x][gridPoint.y] = newPoint;
			}
		}
	}
	return samplePoints;
}