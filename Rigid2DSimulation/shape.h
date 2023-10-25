#pragma once
#include "tile.h"
#include "aabb.h"

struct Cur;
struct Shape {
	bool isCircle = false;
	double r = 0;
	double ang = 0;
	double area = 0;
	vector2 o_rel, o;
	vector<vector2> verticesRelative, vertices;

	Shape(double _r, vector2 _o_rel = {});
	Shape(vector<vector2> const& _verticesRelative);
	void Save(FILE* f) const;
	Shape(FILE* f);

	vector2 generateRandomPointInside() const;
	bool isInside(vector2 p) const;
	aabb getBoundingBox() const;
};

vector2 generateRadomPointInTriangle(vector2 v0, vector2 v1, vector2 v2);
bool isInsideTriangle(vector2 v, vector2 v0, vector2 v1, vector2 v2);
vector<vector2> getRectVerticesByCenter(double w, double h);
