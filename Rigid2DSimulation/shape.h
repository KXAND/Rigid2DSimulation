#pragma once
#include "tile.h"
#include "aabb.h"

struct Cur;
struct Shape {
	bool ball = false;
	double r = 0;
	double ang = 0;
	double area = 0;
	vector2 o_rel, o;
	vector<vector2> vs_rel, vs;

	Shape(double _r, vector2 _o_rel = {});
	Shape(vector<vector2> const& _vs_rel);
	void save(FILE* f) const;
	Shape(FILE* f);

	vector2 rnd_rel() const;
	bool inside(vector2 p) const;
	aabb bounding_box() const;
};

vector2 rnd_tri(vector2 v0, vector2 v1, vector2 v2);
bool inside_tri(vector2 v, vector2 v0, vector2 v1, vector2 v2);
vector<vector2> rect(double w, double h);
