#include "Aabb.h"

Aabb::Aabb(Aabb const& firstBox, Aabb const& secondBox) {
	x0 = min(firstBox.x0, secondBox.x0);
	x1 = max(firstBox.x1, secondBox.x1);
	y0 = min(firstBox.y0, secondBox.y0);
	y1 = max(firstBox.y1, secondBox.y1);
}
void Aabb::Expand(Vector2 point) {
	x0 = min(point.x, x0);
	x1 = max(point.x, x1);
	y0 = min(point.y, y0);
	y1 = max(point.y, y1);
}

bool IsOverlap(Aabb const& box0, Aabb const& box1) {
	bool hasXOverlap = max(box0.x0, box1.x0) <= min(box0.x1, box1.x1);
	bool hasYOverlap = max(box0.y0, box1.y0) <= min(box0.y1, box1.y1);
	return hasXOverlap && hasYOverlap;
}
