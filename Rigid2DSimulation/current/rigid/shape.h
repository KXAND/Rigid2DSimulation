#pragma once
#include "tile.h"
#include "aabb.h"

struct SimulationApp;
/// <summary>
/// 定义一个图形
/// </summary>
struct Shape
{
private:

	vector2 GenerateRandomPointInTriangle(vector2 v0, vector2 v1, vector2 v2) const;
	bool IsInsideTriangle(vector2 v, vector2 v0, vector2 v1, vector2 v2) const;
public:
	bool isCircle = false;
	double r = 0;
	double radian = 0;
	double area = 0;
	vector2 oRelative;
	vector2 o;
	vector<vector2> verticesRelative, vertices;

	Shape(double _r, vector2 _o_rel = {});
	Shape(vector<vector2> const& _verticesRelative);
	Shape(FILE* f);
	void Save(FILE* f) const;

	vector2 GenerateRandomPointInside() const;
	bool IsInside(vector2 p) const;
	aabb GetBoundingBox() const;
};

vector<vector2> GetRectVerticesByCenter(double w, double h);


