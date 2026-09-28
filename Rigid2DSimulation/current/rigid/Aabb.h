#pragma once
#include "Vector2.h"

/// <summary>
/// aabb包围盒
/// </summary>
struct Aabb
{
    double x0 = DBL_MAX, x1 = -DBL_MAX;
    double y0 = DBL_MAX, y1 = -DBL_MAX;

    Aabb() = default;
    Aabb(double x0, double x1, double y0, double y1) : x0(x0), x1(x1), y0(y0), y1(y1) {}
    Aabb(Aabb const& firstBox, Aabb const& secondBox);

    void Expand(Vector2 point);
};
bool IsOverlap(Aabb const& box0, Aabb const& box1);
