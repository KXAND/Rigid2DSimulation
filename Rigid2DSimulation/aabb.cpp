#include "aabb.h"

aabb::aabb(aabb const& b0, aabb const& b1) {
	x0 = min(b0.x0, b1.x0);
	x1 = max(b0.x1, b1.x1);
	y0 = min(b0.y0, b1.y0);
	y1 = max(b0.y1, b1.y1);
}
void aabb::expand(vector2 v) {
	x0 = min(v.x, x0);
	x1 = max(v.x, x1);
	y0 = min(v.y, y0);
	y1 = max(v.y, y1);
}

bool isOverlap(aabb const& box0, aabb const& box1) {
	bool bx = max(box0.x0, box1.x0) <= min(box0.x1, box1.x1);
	bool by = max(box0.y0, box1.y0) <= min(box0.y1, box1.y1);
	return bx && by;
}
