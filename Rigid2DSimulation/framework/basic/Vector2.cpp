#include "Vector2.h"

Vector2::Vector2(wstring const& text)
{
    int index = 0;
    x = FetchNumber(index, text);
    y = FetchNumber(index, text);
}

wstring ToWString(Vector2 vector)
{
    return L"(" + FormatFixed2(vector.x) + L"," + FormatFixed2(vector.y) + L")";
}

bool IsInside(Vector2 vector, Rect rectangle)
{
    return
        IsBetween(vector.x, rectangle.GetLeft(), rectangle.GetRight()) &&
        IsBetween(vector.y, rectangle.GetTop(), rectangle.GetBottom());
}

/// <summary>0__
/// <summary>1__
/// <summary>2__
/// <summary>3__
/// <summary>4__
/// <summary>5__
Vector2 GetPositionOnBezier(vector<Vector2> controlPoints, double t)
{
    /// <summary>6__
    if (controlPoints.size() == 2)
    {
        return controlPoints[0] * (1 - t) + controlPoints[1] * t;
    }

    vector<Vector2> firstControlPoints;
    vector<Vector2> secondControlPoints;
    int controlPointCount = int(controlPoints.size());
    for (int index = 0; index < controlPointCount; ++index)
    {
        if (index != 0)
        {
            secondControlPoints.push_back(controlPoints[index]);
        }
        if (index != controlPoints.size() - 1)
        {
            firstControlPoints.push_back(controlPoints[index]);
        }
    }
    return GetPositionOnBezier(firstControlPoints, t) * (1 - t) + GetPositionOnBezier(secondControlPoints, t) * t;
}

double GetDistanceSquare(Vector2 firstPoint, Vector2 secondPoint, Vector2 point)
{
    if (firstPoint == secondPoint)
    {
        return (firstPoint - point).GetLengthSquared();
    }

    double firstDistanceSquare = (point - firstPoint).GetLengthSquared();
    double secondDistanceSquare = (point - secondPoint).GetLengthSquared();
    double segmentLengthSquare = (firstPoint - secondPoint).GetLengthSquared();
    double distanceDifference = firstDistanceSquare - secondDistanceSquare;
    return
        segmentLengthSquare + secondDistanceSquare < firstDistanceSquare ? secondDistanceSquare : /// <summary>7__
        segmentLengthSquare + firstDistanceSquare < secondDistanceSquare ? firstDistanceSquare :
        (firstDistanceSquare + secondDistanceSquare) / 2 - segmentLengthSquare / 4 - distanceDifference * distanceDifference / (4 * segmentLengthSquare); /// <summary>8__
}
