#pragma once
#include "Vector2.h"

/// <summary>
/// aabb°üÎ§ºÐ
/// </summary>
struct aabb {
	double x0 = DBL_MAX, x1 = -DBL_MAX;
	double y0 = DBL_MAX, y1 = -DBL_MAX;

	aabb() = default;
	aabb(double x0, double x1, double y0, double y1) :
		x0(x0), x1(x1), y0(y0), y1(y1) {}
	aabb(aabb const& b0, aabb const& b1);

	void Expand(Vector2 v);
};
bool IsOverlap(aabb const& b0, aabb const& b1);

