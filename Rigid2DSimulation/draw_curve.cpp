#include "draw_curve.h"
#include "draw_px_seg.h"

void draw_eclipse_frame(tile& targetTile, dbuf& zBuffer, double depth,
	vector2 center, double axisX, double axisY, dRect viewPort, dColor color, int n) {

	auto p0 = center + vector2(axisX, 0);
	for (int i = (0); i < n-1; ++i) 
	{
		double phi = 2 * PI * (i + 1) / n;
		auto p1 = center + vector2(axisX, axisY) * vector2(cos(phi), sin(phi));
		drawLineSegment(targetTile, zBuffer, p0, p1, depth, viewPort, color);
		p0 = p1;
	}
	auto p1 = center + vector2(axisX, 0);
	drawLineSegment(targetTile, zBuffer, p0, p1, depth, viewPort, color);
}
