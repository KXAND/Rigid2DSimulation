#include "draw_curve.h"
#include "draw_px_seg.h"

void drawEllipseBorder(tile& targetTile, dbuf& zBuffer, double depth,
	vector2 center, double a, double b, dRect viewPort, dColor color, int n) 
{
	// 参数方程 （x，y）=(h + a cos t,k + b sin t)
	auto p0 = center + vector2(a, 0);
	for (int i = 0; i < n-1; ++i) 
	{
		double phi = 2 * PI * (i + 1) / n;
		auto p1 = center + vector2(a, b) * vector2(cos(phi), sin(phi));
		drawLineSegment(targetTile, zBuffer, p0, p1, depth, viewPort, color);
		p0 = p1;
	}
	auto p1 = center + vector2(a, 0);
	drawLineSegment(targetTile, zBuffer, p0, p1, depth, viewPort, color);
}
