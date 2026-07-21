#include "DrawCurve.h"
#include "DrawLineSegment.h"

void drawEllipseBorder(Tile& targetTile, DepthBuffer& zBuffer, double depth,
	Vector2 center, double a, double b, DRect viewPort, Color color, int n)
{
	// 参数方程 （x，y）=(h + a cos t,k + b sin t)
	auto p0 = center + Vector2(a, 0);
	for (int i = 0; i < n-1; ++i)
	{
		double phi = 2 * kPi * (i + 1) / n;
		auto p1 = center + Vector2(a, b) * Vector2(cos(phi), sin(phi));
		drawLineSegment(targetTile, zBuffer, p0, p1, depth, viewPort, color);
		p0 = p1;
	}
	auto p1 = center + Vector2(a, 0);
	drawLineSegment(targetTile, zBuffer, p0, p1, depth, viewPort, color);
}

