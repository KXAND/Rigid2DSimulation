#pragma once
#include "Vector2.h"

struct Body;
struct Shape;
struct Collision
{
    Body* body0 = NULL;
    Body* body1 = NULL;
    double depth = DBL_MAX;
    double difference = DBL_MAX;
    Vector2 normal, contactPoint;
    void Simulate(bool useEqualPositionCorrection);
};

void Collide(Body& b0, Body& b1, vector<std::shared_ptr<Collision>>& out, double parallelEpsilon);
// 假设 b0, b1 已经被填入了。
/***need be move to private↓***/
bool CollideShapes(Shape& firstShape, Shape& secondShape, Collision& collision, bool reverse);
void FindClosestContactVertex(Shape& firstShape, Shape& secondShape, Vector2& contactPoint, double& closestDistanceSquared);
bool CollideBalls(Shape& firstShape, Shape& secondShape, Collision& collision);
double CalculateBallCollisionDepth(Vector2 normal, Vector2 origin, Shape const& shape, Vector2& contactPoint);
double CalculatePolygonCollisionDepth(Vector2 normal, Vector2 origin, Shape const& shape, Vector2& contactPoint);
