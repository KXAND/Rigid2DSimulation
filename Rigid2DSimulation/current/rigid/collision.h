#pragma once
#include "vector2.h"

struct SimulationApp;
struct Body;
struct Shape;
struct Collision
{
	Body* body0 = NULL;
	Body* body1 = NULL;
	double d = DBL_MAX;
	double diff = DBL_MAX;
	Vector2 n, c;
	void Render(SimulationApp& cur) const;
	void Simulate(bool useEqualPositionCorrection);
};

void Collide(Body& b0, Body& b1, vector<std::shared_ptr<Collision>>& out, double parallelEpsilon);
// 假设 b0, b1 已经被填入了。
/***need be move to private↓***/
bool collide(Shape& sh0, Shape& sh1, Collision& out, bool reverse);
void FindClosestContactVertex(Shape& sh0, Shape& sh1, Vector2& c, double& dsqr_close);
bool CollideBalls(Shape& sh0, Shape& sh1, Collision& out);
double CalculateBallCollisionDepth(Vector2 n, Vector2 o, Shape const& sh, Vector2& c);
double CalculatePolygonCollisionDepth(Vector2 n, Vector2 o, Shape const& sh, Vector2& c);


