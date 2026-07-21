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
bool preDrawLineSegment(DVector2& pa, DVector2& pb, DRect viewPort) {
	long long x0 = viewPort.left();
	long long x1 = viewPort.right() - 1;
	long long y0 = viewPort.top();
	long long y1 = viewPort.bottom() - 1;

	// 线性插值
	// 下面假设了不会发生除以 0 的问题。
	bool b0 = pa.x < x0 && pb.x < x0;
	bool b1 = pa.x > x1 && pb.x > x1;
	if (b0 || b1) { return false; }
	ClipLineEndpointAtX(pa, pb, x0, BoundarySide::Minimum);
	ClipLineEndpointAtX(pa, pb, x1, BoundarySide::Maximum);
	ClipLineEndpointAtX(pb, pa, x0, BoundarySide::Minimum);
	ClipLineEndpointAtX(pb, pa, x1, BoundarySide::Maximum);

	b0 = pa.y < y0 && pb.y < y0;
	b1 = pa.y > y1 && pb.y > y1;
	if (b0 || b1) { return false; }
	ClipLineEndpointAtY(pa, pb, y0, BoundarySide::Minimum);
	ClipLineEndpointAtY(pa, pb, y1, BoundarySide::Maximum);
	ClipLineEndpointAtY(pb, pa, y0, BoundarySide::Minimum);
	ClipLineEndpointAtY(pb, pa, y1, BoundarySide::Maximum);
	return true;
}
void drawLineSegment(tile& targetTile, dbuf& zBuffer,
	DVector2 pa, DVector2 pb, double d, DRect viewPort, dColor color)
{
	if (viewPort.w <= 0 || viewPort.h <= 0) { return; }
	if (!preDrawLineSegment(pa, pb, viewPort)) { return; }

	bool changed = false;
	int x = pa.x; int y = pa.y;
	int dx = abs(pb.x - pa.x);
	int dy = abs(pb.y - pa.y);
	int sx = (pb.x > pa.x) ? 1 : -1;
	int sy = (pb.y > pa.y) ? 1 : -1;
	if (dy > dx) { swap(dx, dy); changed = true; }

	int e = dx;
	int id = y * targetTile.width + x;
	if (zBuffer[id] <= d) { zBuffer[id] = d; targetTile.colors[id] = color; }

	for (int i = (0), iEnd = int(dx); i < iEnd; ++i) {
		e -= 2 * dy;
		if (changed) { y += sy; }
		else { x += sx; }
		if (e < 0) {
			e += 2 * dx;
			if (changed) { x += sx; }
			else { y += sy; }
		}
		int id = y * targetTile.width + x;
		if (zBuffer[id] <= d) { zBuffer[id] = d; targetTile.colors[id] = color; }
	}
}
void drawLineSegment(tile& targetTile, dbuf& zBuffer,
	Vector2 pa, Vector2 pb, double depth, DRect viewPort, dColor color)
{
	drawLineSegment(targetTile, zBuffer, (DVector2)pa, (DVector2)pb, depth, viewPort, color);
}

