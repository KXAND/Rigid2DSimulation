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

bool ClipTriangleAtHorizontalBoundary(tile& targetTile, dbuf& zBuffer,
    DRect const& viewPort, DVector2 const& firstPoint,
    DVector2 const& middlePoint, DVector2 const& lastPoint, double depth,
    dColor color, long long boundary, BoundarySide boundarySide)
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
        drawTriangle(targetTile, zBuffer, viewPort, firstLastIntersection,
            middleLastIntersection, lastPoint, depth, color);
        return true;
    }

    DVector2 firstMiddleIntersection = middlePoint;
    firstMiddleIntersection.y = boundary;
    firstMiddleIntersection.x += (firstPoint.x - middlePoint.x)
        * (boundary - middlePoint.y) / (firstPoint.y - middlePoint.y);
    drawTriangle(targetTile, zBuffer, viewPort, firstLastIntersection,
        middlePoint, lastPoint, depth, color);
    drawTriangle(targetTile, zBuffer, viewPort, firstLastIntersection,
        middlePoint, firstMiddleIntersection, depth, color);
    return true;
}
}

void subDrawTriangle(tile& dest, dbuf& zBuffer,
	DVector2 pa, DVector2 pb, DVector2 pc, double depth, dColor color, bool same_y)
{
	// 假设 pa, pb, pc  已经交换过了。
	// 用它画斜的长方形有的时候会出现裂痕，不知道能不能修复。
	if (pa.y == pb.y)  return;
	int dy = pb.y > pa.y ? 1 : -1;
	int dx = pc.x > pb.x ? 1 : -1;
	for (int y = pa.y; y != pb.y + dy; y += dy)
	{
		int xb = pa.x * (pb.y - y) + pb.x * (y - pa.y); xb /= (pb.y - pa.y);
		int xc = pa.x * (pb.y - y) + pc.x * (y - pa.y); xc /= (pb.y - pa.y);
		for (int x = xb; x != xc + dx; x += dx)
		{
			int dp = same_y ? y * dest.width + x : x * dest.width + y;
			if (zBuffer[dp] <= depth) { zBuffer[dp] = depth; dest.colors[dp] = color; }
		}
	}
}
void drawTriangle(tile& targetTile, dbuf& zBuffer, DRect const& viewPort,
	DVector2 pa, DVector2 pb, DVector2 pc, double depth, dColor color)
{
	// 如果可以让这里写得好看有条理，容易修改且不容易出 bug 就好了。
	// @todo:考虑笔记中的 SH 算法

	// 保证x轴上左到右是　cba　的顺序
	// 这里有点低效，重复比较了，但是总的影响不大。
	if (pa.x < pb.x) swap(pa, pb);
	if (pa.x < pc.x) swap(pa, pc);
	if (pb.x < pc.x) swap(pb, pc);

	if (pa.x == pc.x) { return; } // 三角形三点不可能共线
	int dx = pa.x - pc.x;

	// 用屏幕边切割图形
	if (ClipTriangleAtHorizontalBoundary(targetTile, zBuffer, viewPort, pa, pb,
		pc, depth, color, (long long)viewPort.right() - 1,
		BoundarySide::Maximum))
	{
		return;
	}
	if (ClipTriangleAtHorizontalBoundary(targetTile, zBuffer, viewPort, pc, pb,
		pa, depth, color, (long long)viewPort.left(), BoundarySide::Minimum))
	{
		return;
	}

	if (pa.y < pb.y) { swap(pa, pb); }
	if (pa.y < pc.y) { swap(pa, pc); }
	if (pb.y < pc.y) { swap(pb, pc); }
	if (pa.y == pc.y) { return; }
	int dy = pa.y - pc.y;
	if (ClipTriangleAtHorizontalBoundary(targetTile, zBuffer, viewPort, pa, pb,
		pc, depth, color, (long long)viewPort.bottom() - 1,
		BoundarySide::Maximum))
	{
		return;
	}
	if (ClipTriangleAtHorizontalBoundary(targetTile, zBuffer, viewPort, pc, pb,
		pa, depth, color, (long long)viewPort.top(), BoundarySide::Minimum))
	{
		return;
	}

	// 这里是出于数值稳定性的考虑交换一下方向，否则如果三角形很扁的话会奇怪。
	bool y_big = dy > dx;
	if (!y_big)
	{
		if (pa.x < pb.x) { swap(pa, pb); }
		if (pa.x < pc.x) { swap(pa, pc); }
		if (pb.x < pc.x) { swap(pb, pc); }
		swap(pa.x, pa.y); swap(pb.x, pb.y); swap(pc.x, pc.y);
	}
	DVector2 pd = pb;
	pd.x = pa.x * (pb.y - pc.y) + pc.x * (pa.y - pb.y); pd.x /= pa.y - pc.y;
	// @ques：不是很理解这里问什么要做两遍
	subDrawTriangle(targetTile, zBuffer, pa, pb, pd, depth, color, y_big);
	subDrawTriangle(targetTile, zBuffer, pc, pb, pd, depth, color, y_big);
}
void drawTriangle(tile& targetTile, dbuf& zBuffer, DRect const& viewPort,
	Vector2 pa, Vector2 pb, Vector2 pc, double depth, dColor color)
{
	drawTriangle(targetTile, zBuffer, viewPort, (DVector2) pa, (DVector2) pb, (DVector2) pc, depth, color);
}

