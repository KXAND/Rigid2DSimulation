#pragma once
#include "Tile.h"
#include "Aabb.h"

struct SimulationApp;
/// <summary>
/// 定义一个图形
/// </summary>
struct Shape
{
private:

	Vector2 GenerateRandomPointInTriangle(Vector2 v0, Vector2 v1, Vector2 v2) const;
	bool IsInsideTriangle(Vector2 v, Vector2 v0, Vector2 v1, Vector2 v2) const;
public:
	bool isCircle = false;
	double radius = 0;
	double radian = 0;
	double area = 0;
	Vector2 relativePosition;
	Vector2 position;
	vector<Vector2> verticesRelative, vertices;

	Shape(double radius, Vector2 relativePosition = {});
	Shape(vector<Vector2> const& verticesRelative);
	Shape(FILE* file);
	void Save(FILE* file) const;

	Vector2 GenerateRandomPointInside() const;
	bool IsInside(Vector2 p) const;
	Aabb GetBoundingBox() const;
};

vector<Vector2> GetRectVerticesByCenter(double width, double height);


