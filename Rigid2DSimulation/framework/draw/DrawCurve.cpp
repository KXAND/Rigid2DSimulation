#include "DrawCurve.h"
#include "DrawLineSegment.h"

void DrawEllipseBorder(Tile& targetTile, DepthBuffer& zBuffer, double depth,
                       Vector2 center, double horizontalRadius, double verticalRadius, DRect viewport, Color color, int segmentCount)
{
    // 参数方程 （x，y）=(h + horizontalRadius cos t,k + verticalRadius sin t)
    auto p0 = center + Vector2(horizontalRadius, 0);
    for (int i = 0; i < segmentCount - 1; ++i)
    {
        double phi = 2 * kPi * (i + 1) / segmentCount;
        auto p1 = center + Vector2(horizontalRadius, verticalRadius) * Vector2(cos(phi), sin(phi));
        DrawLineSegment(targetTile, zBuffer, p0, p1, depth, viewport, color);
        p0 = p1;
    }
    auto p1 = center + Vector2(horizontalRadius, 0);
    DrawLineSegment(targetTile, zBuffer, p0, p1, depth, viewport, color);
}
