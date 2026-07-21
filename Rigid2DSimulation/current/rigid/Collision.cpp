#include "Collision.h"
#include "SimulationApp.h"
#include "Background.h"
#include "Body.h"
#include "Shape.h"
#include "DrawGeometry.h"

//	疑似废弃
void Collision::Render(SimulationApp& simulationApp) const
{
	DrawEllipse(simulationApp.screen, simulationApp.depthBuffer, 100, contactPoint, 5, 5, simulationApp.background->GetViewport(), Color::YELLOW);
}
void Collision::Simulate(bool equalResponse)
{
	/*命名约定：
		?????c
		???壺1??2
	*/

	double elasticity = min(body0->elasticity, body1->elasticity);
	double frictionDynamic = min(body0->frictionDynamic, body1->frictionDynamic);
	double frictionStatic = min(body0->frictionStatic, body1->frictionStatic);

	// 接触点处两个物体的切向量（tangent vector）和对应速度
	Vector2 tanC1 = contactPoint - body0->position;
	Vector2 tanC2 = contactPoint - body1->position;
	tanC1 = Vector2(-tanC1.y, tanC1.x);//逆时针转90度
	tanC2 = Vector2(-tanC2.y, tanC2.x);
	Vector2 velocity0 = body0->velocity + body0->velocityAngular * tanC1;
	Vector2 velocity1 = body1->velocity + body1->velocityAngular * tanC2;

	// 法线方向
	// @todo：这里需要补物理知识
	double tanC0normal = Dot(tanC1, normal);
	double tanC1normal = Dot(tanC2, normal);
	double inverse_i0 = body0->invMass + tanC0normal * tanC0normal * body0->invInertia;
	double inverse_i1 = body1->invMass + tanC1normal * tanC1normal * body1->invInertia;
	double impulseNormal = (1 + elasticity) * max(0.0, Dot(velocity0 - velocity1, normal)) / (inverse_i0 + inverse_i1); // 法向方向的冲量

	Vector2 impulse;
	impulse = -impulseNormal * normal;
	body0->velocity += impulse * body0->invMass;
	body1->velocity -= impulse * body1->invMass;
	body0->velocityAngular += Dot(impulse, tanC1) * body0->invInertia;
	body1->velocityAngular -= Dot(impulse, tanC2) * body1->invInertia;

	Vector2 updatedVelocity0 = body0->velocity + body0->velocityAngular * tanC1;
	Vector2 updatedVelocity1 = body1->velocity + body1->velocityAngular * tanC2;

	double staticFrictionImpulse = frictionStatic * impulseNormal;
	double dynamicFrictionImpulse = frictionDynamic * impulseNormal;


	// 切线方向
	Vector2 tangentDirection = Vector2(-normal.y, normal.x);
	double impulseTangential = Dot(updatedVelocity0 - updatedVelocity1, tangentDirection);
	if (impulseTangential < 0)
	{
		impulseTangential = -impulseTangential; tangentDirection = -tangentDirection;
	}
	double tanCtan0 = Dot(tanC1, tangentDirection);// tantan？？？
	double tanCtan1 = Dot(tanC2, tangentDirection);
	double inverse_it0 = body0->invMass + tanCtan0 * tanCtan0 * body0->invInertia;
	double inverse_it1 = body1->invMass + tanCtan1 * tanCtan1 * body1->invInertia;
	impulseTangential /= inverse_it0 + inverse_it1;

	if (impulseTangential < staticFrictionImpulse)
	{
		impulse = -impulseTangential * tangentDirection;
	}
	else
	{
		impulse = -dynamicFrictionImpulse * tangentDirection;
	}

	body0->velocity += impulse * body0->invMass;
	body1->velocity -= impulse * body1->invMass;
	body0->velocityAngular += Dot(impulse, tanC1) * body0->invInertia;
	body1->velocityAngular -= Dot(impulse, tanC2) * body1->invInertia;

	if (equalResponse)
	{
		if (body0->invMass == 0) { body1->position += normal * depth; }
		else if (body1->invMass == 0) { body0->position -= normal * depth; }
		else
		{
			body0->position -= normal * depth / 2;
			body1->position += normal * depth / 2;
		}
		return;
	}
	// 我是真的不知道这个有没有道理。
	double sum = body0->invMass + body1->invMass;
	body0->position -= normal * depth * body0->invMass / sum;
	body1->position += normal * depth * body1->invMass / sum;
}

void Collide(Body& b0, Body& b1, vector<std::shared_ptr<Collision>>& out, double parallelEpsilon)
{
	// 不可能发生碰撞的情况
	if (b0.invMass == 0 && b1.invMass == 0) { return; }
	if (!IsOverlap(b0.box, b1.box)) { return; }

	for (auto& shape0 : b0.shapes)
		for (auto& shape1 : b1.shapes)
		{
			Collision collision;

			collision.body0 = &b0; collision.body1 = &b1;
			if (shape0->isCircle && shape1->isCircle)
			{
				// 都是球
				if (!CollideBalls(*shape0, *shape1, collision)) { continue; }
			}
			else
			{
				if (!CollideShapes(*shape0, *shape1, collision, false)) { continue; }
				if (!CollideShapes(*shape1, *shape0, collision, true)) { continue; }
			}


			// 都不是圆，检查最近距离是否小于阈值
			if (!shape0->isCircle && !shape1->isCircle && collision.difference < parallelEpsilon)
			{
				double distanceSquareClosest = DBL_MAX;
				FindClosestContactVertex(*shape0, *shape1, collision.contactPoint, distanceSquareClosest);
				FindClosestContactVertex(*shape1, *shape0, collision.contactPoint, distanceSquareClosest);
			}

			out.push_back(std::make_shared<Collision>(collision));
		}
}

/***need be move to private↓***/

bool CollideShapes(Shape& firstShape, Shape& secondShape, Collision& collision, bool reverse)
{
	Vector2 contactPoint;
	if (firstShape.isCircle)
	{
		for (int i = (0), iEnd = int(secondShape.vertices.size()); i < iEnd; ++i)
		{
			Vector2 normal = (secondShape.vertices[i] - firstShape.position).GetNormalized();
			if (normal.IsZero()) { normal = Vector2(0, 1); }
			auto d = CalculatePolygonCollisionDepth(normal, firstShape.position + normal * firstShape.radius, secondShape, contactPoint);
			if (d < 0) { return false; }
			if (d < collision.depth)
			{
				collision.difference = collision.depth - d;
				collision.depth = d;
				collision.contactPoint = contactPoint;
				collision.normal = reverse ? -normal : normal;
			}
		}
		return true;
	}

	for (int i = (0), iEnd = int(firstShape.vertices.size()); i < iEnd; ++i)
	{
		int j = (i + 1) % firstShape.vertices.size();
		Vector2 normal = firstShape.vertices[j] - firstShape.vertices[i];
		normal = Vector2(normal.y, -normal.x).GetNormalized();
		auto d = secondShape.isCircle ?
			CalculateBallCollisionDepth(normal, firstShape.vertices[i], secondShape, contactPoint) :
			CalculatePolygonCollisionDepth(normal, firstShape.vertices[i], secondShape, contactPoint);
		if (d < 0) { return false; }
		if (d < collision.depth)
		{
			collision.difference = collision.depth - d;
			collision.depth = d;
			collision.contactPoint = contactPoint;
			collision.normal = reverse ? -normal : normal;
		}
	} return true;
}
void FindClosestContactVertex(Shape& firstShape, Shape& secondShape, Vector2& contactPoint, double& closestDistanceSquared)
{
	for (int i = (0), iEnd = int(firstShape.vertices.size()); i < iEnd; ++i)
	{
		int j = (i + 1) % firstShape.vertices.size();
		for (auto v : secondShape.vertices)
		{
			//算出该顶点到 firstShape 当前顶点和下一个顶点所构成的线段的距离平方
			double distanceSquare = GetDistanceSquare(firstShape.vertices[i], firstShape.vertices[j], v);
			if (distanceSquare < closestDistanceSquared)
			{
				closestDistanceSquared = distanceSquare;
				contactPoint = v;
			}
		}
	}
}
bool CollideBalls(Shape& firstShape, Shape& secondShape, Collision& collision)
{
	Vector2 o10 = secondShape.position - firstShape.position;
	double d = firstShape.radius + secondShape.radius - o10.GetLength();
	if (d < 0) { return false; }
	collision.depth = d;
	collision.normal = o10.IsZero() ? Vector2(0, 1) : o10.GetNormalized();
	if (firstShape.radius < secondShape.radius)
	{
		collision.contactPoint = firstShape.position + firstShape.radius * collision.normal;
	}
	else
	{
		collision.contactPoint = secondShape.position - secondShape.radius * collision.normal;
	} return true;
}
double CalculateBallCollisionDepth(Vector2 normal, Vector2 origin, Shape const& sh, Vector2& contactPoint)
{
	auto tmp = Dot(sh.position - origin, normal) - sh.radius;
	contactPoint = sh.position - sh.radius * normal; return -tmp;
}
double CalculatePolygonCollisionDepth(Vector2 normal, Vector2 origin, Shape const& sh, Vector2& contactPoint)
{
	double out = DBL_MAX;
	for (auto v : sh.vertices)
	{
		auto tmp = Dot(v - origin, normal);
		if (tmp < out)
		{
			out = tmp; contactPoint = v;
		}
	} return -out;
}



