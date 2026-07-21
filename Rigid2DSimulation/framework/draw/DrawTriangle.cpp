#include "DrawTriangle.h"

namespace
{
enum class BoundarySide
{
    Minimum,
    Maximum
};

bool IsOutsideBoundary(DVector2 const& point, long long boundary,
    BoundarySide boundarySide)
{
    return boundarySide == BoundarySide::Minimum
        ? point.y < boundary
        : point.y > boundary;
}

bool ClipTriangleAtHorizontalBoundary(Tile& targetTile, DepthBuffer& zBuffer,
    DRect const& viewport, DVector2 const& firstPoint,
    DVector2 const& middlePoint, DVector2 const& lastPoint, double depth,
    Color color, long long boundary, BoundarySide boundarySide)
{
    if (IsOutsideBoundary(lastPoint, boundary, boundarySide))
    {
        return true;
    }

    if (!IsOutsideBoundary(firstPoint, boundary, boundarySide))
    {
        return false;
    }

    DVector2 firstLastIntersection = lastPoint;
    firstLastIntersection.y = boundary;
    firstLastIntersection.x += (firstPoint.x - lastPoint.x)
        * (boundary - lastPoint.y) / (firstPoint.y - lastPoint.y);

    if (IsOutsideBoundary(middlePoint, boundary, boundarySide))
    {
        DVector2 middleLastIntersection = lastPoint;
        middleLastIntersection.y = boundary;
        middleLastIntersection.x += (middlePoint.x - lastPoint.x)
            * (boundary - lastPoint.y) / (middlePoint.y - lastPoint.y);
        DrawTriangle(targetTile, zBuffer, viewport, firstLastIntersection,
            middleLastIntersection, lastPoint, depth, color);
        return true;
    }

    DVector2 firstMiddleIntersection = middlePoint;
    firstMiddleIntersection.y = boundary;
    firstMiddleIntersection.x += (firstPoint.x - middlePoint.x)
        * (boundary - middlePoint.y) / (firstPoint.y - middlePoint.y);
    DrawTriangle(targetTile, zBuffer, viewport, firstLastIntersection,
        middlePoint, lastPoint, depth, color);
    DrawTriangle(targetTile, zBuffer, viewport, firstLastIntersection,
        middlePoint, firstMiddleIntersection, depth, color);
    return true;
}
}

void DrawTriangleHalf(Tile& targetTile, DepthBuffer& zBuffer,
	DVector2 firstPoint, DVector2 middlePoint, DVector2 lastPoint, double depth, Color color, bool isYAxisMajor)
{
	// 假设 firstPoint, middlePoint, lastPoint  已经交换过了。
	// 用它画斜的长方形有的时候会出现裂痕，不知道能不能修复。
	if (firstPoint.y == middlePoint.y)  return;
	int dy = middlePoint.y > firstPoint.y ? 1 : -1;
	int dx = lastPoint.x > middlePoint.x ? 1 : -1;
	for (int y = firstPoint.y; y != middlePoint.y + dy; y += dy)
	{
		int middleEdgeX = firstPoint.x * (middlePoint.y - y) + middlePoint.x * (y - firstPoint.y); middleEdgeX /= (middlePoint.y - firstPoint.y);
		int lastEdgeX = firstPoint.x * (middlePoint.y - y) + lastPoint.x * (y - firstPoint.y); lastEdgeX /= (middlePoint.y - firstPoint.y);
		for (int x = middleEdgeX; x != lastEdgeX + dx; x += dx)
		{
			int depthIndex = isYAxisMajor ? y * targetTile.width + x : x * targetTile.width + y;
			if (zBuffer[depthIndex] <= depth) { zBuffer[depthIndex] = depth; targetTile.colors[depthIndex] = color; }
		}
	}
}
void DrawTriangle(Tile& targetTile, DepthBuffer& zBuffer, DRect const& viewport,
	DVector2 firstPoint, DVector2 middlePoint, DVector2 lastPoint, double depth, Color color)
{
	// 如果可以让这里写得好看有条理，容易修改且不容易出 bug 就好了。
	// @todo:考虑笔记中的 SH 算法

	// 保证x轴上左到右是　cba　的顺序
	// 这里有点低效，重复比较了，但是总的影响不大。
	if (firstPoint.x < middlePoint.x) swap(firstPoint, middlePoint);
	if (firstPoint.x < lastPoint.x) swap(firstPoint, lastPoint);
	if (middlePoint.x < lastPoint.x) swap(middlePoint, lastPoint);

	if (firstPoint.x == lastPoint.x) { return; } // 三角形三点不可能共线
	int dx = firstPoint.x - lastPoint.x;

	// 用屏幕边切割图形
	if (ClipTriangleAtHorizontalBoundary(targetTile, zBuffer, viewport, firstPoint, middlePoint,
		lastPoint, depth, color, (long long)viewport.GetRight() - 1,
		BoundarySide::Maximum))
	{
		return;
	}
	if (ClipTriangleAtHorizontalBoundary(targetTile, zBuffer, viewport, lastPoint, middlePoint,
		firstPoint, depth, color, (long long)viewport.GetLeft(), BoundarySide::Minimum))
	{
		return;
	}

	if (firstPoint.y < middlePoint.y) { swap(firstPoint, middlePoint); }
	if (firstPoint.y < lastPoint.y) { swap(firstPoint, lastPoint); }
	if (middlePoint.y < lastPoint.y) { swap(middlePoint, lastPoint); }
	if (firstPoint.y == lastPoint.y) { return; }
	int dy = firstPoint.y - lastPoint.y;
	if (ClipTriangleAtHorizontalBoundary(targetTile, zBuffer, viewport, firstPoint, middlePoint,
		lastPoint, depth, color, (long long)viewport.GetBottom() - 1,
		BoundarySide::Maximum))
	{
		return;
	}
	if (ClipTriangleAtHorizontalBoundary(targetTile, zBuffer, viewport, lastPoint, middlePoint,
		firstPoint, depth, color, (long long)viewport.GetTop(), BoundarySide::Minimum))
	{
		return;
	}

	// 这里是出于数值稳定性的考虑交换一下方向，否则如果三角形很扁的话会奇怪。
	bool isYAxisMajor = dy > dx;
	if (!isYAxisMajor)
	{
		if (firstPoint.x < middlePoint.x) { swap(firstPoint, middlePoint); }
		if (firstPoint.x < lastPoint.x) { swap(firstPoint, lastPoint); }
		if (middlePoint.x < lastPoint.x) { swap(middlePoint, lastPoint); }
		swap(firstPoint.x, firstPoint.y); swap(middlePoint.x, middlePoint.y); swap(lastPoint.x, lastPoint.y);
	}
	DVector2 splitPoint = middlePoint;
	splitPoint.x = firstPoint.x * (middlePoint.y - lastPoint.y) + lastPoint.x * (firstPoint.y - middlePoint.y); splitPoint.x /= firstPoint.y - lastPoint.y;
	// @ques：不是很理解这里问什么要做两遍
	DrawTriangleHalf(targetTile, zBuffer, firstPoint, middlePoint, splitPoint, depth, color, isYAxisMajor);
	DrawTriangleHalf(targetTile, zBuffer, lastPoint, middlePoint, splitPoint, depth, color, isYAxisMajor);
}
void DrawTriangle(Tile& targetTile, DepthBuffer& zBuffer, DRect const& viewport,
	Vector2 firstPoint, Vector2 middlePoint, Vector2 lastPoint, double depth, Color color)
{
	DrawTriangle(targetTile, zBuffer, viewport, (DVector2) firstPoint, (DVector2) middlePoint, (DVector2) lastPoint, depth, color);
}

