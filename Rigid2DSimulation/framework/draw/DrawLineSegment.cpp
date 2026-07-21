#include "DrawLineSegment.h"

namespace
{
enum class BoundarySide
{
    Minimum,
    Maximum
};

void ClipLineEndpointAtX(DVector2& endpoint, DVector2 const& otherEndpoint,
    long long boundary, BoundarySide boundarySide)
{
    bool const isOutside = boundarySide == BoundarySide::Minimum
        ? endpoint.x < boundary
        : endpoint.x > boundary;
    if (isOutside)
    {
        endpoint.y += (otherEndpoint.y - endpoint.y) * (boundary - endpoint.x)
            / (otherEndpoint.x - endpoint.x);
        endpoint.x = boundary;
    }
}

void ClipLineEndpointAtY(DVector2& endpoint, DVector2 const& otherEndpoint,
    long long boundary, BoundarySide boundarySide)
{
    bool const isOutside = boundarySide == BoundarySide::Minimum
        ? endpoint.y < boundary
        : endpoint.y > boundary;
    if (isOutside)
    {
        endpoint.x += (otherEndpoint.x - endpoint.x) * (boundary - endpoint.y)
            / (otherEndpoint.y - endpoint.y);
        endpoint.y = boundary;
    }
}
}

// 好像这里经常发生溢出问题，不知能不能处理一下。
// 把 int 换成 long long 了，应该会有帮助。
bool ClipLineSegment(DVector2& firstPoint, DVector2& secondPoint, DRect viewport) {
	long long left = viewport.GetLeft();
	long long right = viewport.GetRight() - 1;
	long long top = viewport.GetTop();
	long long bottom = viewport.GetBottom() - 1;

	// 线性插值
	// 下面假设了不会发生除以 0 的问题。
	bool isOutsideMinimum = firstPoint.x < left && secondPoint.x < left;
	bool isOutsideMaximum = firstPoint.x > right && secondPoint.x > right;
	if (isOutsideMinimum || isOutsideMaximum) { return false; }
	ClipLineEndpointAtX(firstPoint, secondPoint, left, BoundarySide::Minimum);
	ClipLineEndpointAtX(firstPoint, secondPoint, right, BoundarySide::Maximum);
	ClipLineEndpointAtX(secondPoint, firstPoint, left, BoundarySide::Minimum);
	ClipLineEndpointAtX(secondPoint, firstPoint, right, BoundarySide::Maximum);

	isOutsideMinimum = firstPoint.y < top && secondPoint.y < top;
	isOutsideMaximum = firstPoint.y > bottom && secondPoint.y > bottom;
	if (isOutsideMinimum || isOutsideMaximum) { return false; }
	ClipLineEndpointAtY(firstPoint, secondPoint, top, BoundarySide::Minimum);
	ClipLineEndpointAtY(firstPoint, secondPoint, bottom, BoundarySide::Maximum);
	ClipLineEndpointAtY(secondPoint, firstPoint, top, BoundarySide::Minimum);
	ClipLineEndpointAtY(secondPoint, firstPoint, bottom, BoundarySide::Maximum);
	return true;
}
void DrawLineSegment(Tile& targetTile, DepthBuffer& zBuffer,
	DVector2 firstPoint, DVector2 secondPoint, double depth, DRect viewport, Color color)
{
	if (viewport.width <= 0 || viewport.height <= 0) { return; }
	if (!ClipLineSegment(firstPoint, secondPoint, viewport)) { return; }

	bool isYAxisMajor = false;
	int x = firstPoint.x; int y = firstPoint.y;
	int dx = abs(secondPoint.x - firstPoint.x);
	int dy = abs(secondPoint.y - firstPoint.y);
	int stepX = (secondPoint.x > firstPoint.x) ? 1 : -1;
	int stepY = (secondPoint.y > firstPoint.y) ? 1 : -1;
	if (dy > dx) { swap(dx, dy); isYAxisMajor = true; }

	int errorTerm = dx;
	int depthIndex = y * targetTile.width + x;
	if (zBuffer[depthIndex] <= depth) { zBuffer[depthIndex] = depth; targetTile.colors[depthIndex] = color; }

	for (int i = (0), iEnd = int(dx); i < iEnd; ++i) {
		errorTerm -= 2 * dy;
		if (isYAxisMajor) { y += stepY; }
		else { x += stepX; }
		if (errorTerm < 0) {
			errorTerm += 2 * dx;
			if (isYAxisMajor) { x += stepX; }
			else { y += stepY; }
		}
		int depthIndex = y * targetTile.width + x;
		if (zBuffer[depthIndex] <= depth) { zBuffer[depthIndex] = depth; targetTile.colors[depthIndex] = color; }
	}
}
void DrawLineSegment(Tile& targetTile, DepthBuffer& zBuffer,
	Vector2 firstPoint, Vector2 secondPoint, double depth, DRect viewport, Color color)
{
	DrawLineSegment(targetTile, zBuffer, (DVector2)firstPoint, (DVector2)secondPoint, depth, viewport, color);
}

