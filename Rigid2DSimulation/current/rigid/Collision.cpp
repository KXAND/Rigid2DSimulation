#include "Collision.h"
#include "SimulationApp.h"
#include "Background.h"
#include "Body.h"
#include "Shape.h"
#include "DrawGeometry.h"

//	疑似废弃
void Collision::Render(SimulationApp& simulationApp) const
{
	DrawEllipse(simulationApp.screen, simulationApp.depthBuffer, 100, contactPoint, 5, 5, simulationApp.background->GetViewport(), Color::kYellow);
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
	Vector2 contactOffsetPerpendicular0 = contactPoint - body0->position;
	Vector2 contactOffsetPerpendicular1 = contactPoint - body1->position;
	contactOffsetPerpendicular0 = Vector2(-contactOffsetPerpendicular0.y, contactOffsetPerpendicular0.x);//逆时针转90度
	contactOffsetPerpendicular1 = Vector2(-contactOffsetPerpendicular1.y, contactOffsetPerpendicular1.x);
	Vector2 velocity0 = body0->velocity + body0->velocityAngular * contactOffsetPerpendicular0;
	Vector2 velocity1 = body1->velocity + body1->velocityAngular * contactOffsetPerpendicular1;

	// 法线方向
	// @todo：这里需要补物理知识
	double normalMomentArm0 = Dot(contactOffsetPerpendicular0, normal);
	double normalMomentArm1 = Dot(contactOffsetPerpendicular1, normal);
	double normalInvMass0 = body0->invMass + normalMomentArm0 * normalMomentArm0 * body0->invInertia;
	double normalInvMass1 = body1->invMass + normalMomentArm1 * normalMomentArm1 * body1->invInertia;
	double impulseNormal = (1 + elasticity) * max(0.0, Dot(velocity0 - velocity1, normal)) / (normalInvMass0 + normalInvMass1); // 法向方向的冲量

	Vector2 impulse;
	impulse = -impulseNormal * normal;
	body0->velocity += impulse * body0->invMass;
	body1->velocity -= impulse * body1->invMass;
	body0->velocityAngular += Dot(impulse, contactOffsetPerpendicular0) * body0->invInertia;
	body1->velocityAngular -= Dot(impulse, contactOffsetPerpendicular1) * body1->invInertia;

	Vector2 updatedVelocity0 = body0->velocity + body0->velocityAngular * contactOffsetPerpendicular0;
	Vector2 updatedVelocity1 = body1->velocity + body1->velocityAngular * contactOffsetPerpendicular1;

	double staticFrictionImpulse = frictionStatic * impulseNormal;
	double dynamicFrictionImpulse = frictionDynamic * impulseNormal;


	// 切线方向
	Vector2 tangentDirection = Vector2(-normal.y, normal.x);
	double impulseTangential = Dot(updatedVelocity0 - updatedVelocity1, tangentDirection);
	if (impulseTangential < 0)
	{
		impulseTangential = -impulseTangential; tangentDirection = -tangentDirection;
	}
	double tangentMomentArm0 = Dot(contactOffsetPerpendicular0, tangentDirection);// tantan？？？
	double tangentMomentArm1 = Dot(contactOffsetPerpendicular1, tangentDirection);
	double tangentInvMass0 = body0->invMass + tangentMomentArm0 * tangentMomentArm0 * body0->invInertia;
	double tangentInvMass1 = body1->invMass + tangentMomentArm1 * tangentMomentArm1 * body1->invInertia;
	impulseTangential /= tangentInvMass0 + tangentInvMass1;

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
	body0->velocityAngular += Dot(impulse, contactOffsetPerpendicular0) * body0->invInertia;
	body1->velocityAngular -= Dot(impulse, contactOffsetPerpendicular1) * body1->invInertia;

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
			auto depth = CalculatePolygonCollisionDepth(normal, firstShape.position + normal * firstShape.radius, secondShape, contactPoint);
			if (depth < 0) { return false; }
			if (depth < collision.depth)
			{
				collision.difference = collision.depth - depth;
				collision.depth = depth;
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
		auto depth = secondShape.isCircle ?
			CalculateBallCollisionDepth(normal, firstShape.vertices[i], secondShape, contactPoint) :
			CalculatePolygonCollisionDepth(normal, firstShape.vertices[i], secondShape, contactPoint);
		if (depth < 0) { return false; }
		if (depth < collision.depth)
		{
			collision.difference = collision.depth - depth;
			collision.depth = depth;
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
		for (auto vertex : secondShape.vertices)
		{
			//算出该顶点到 firstShape 当前顶点和下一个顶点所构成的线段的距离平方
			double distanceSquare = GetDistanceSquare(firstShape.vertices[i], firstShape.vertices[j], vertex);
			if (distanceSquare < closestDistanceSquared)
			{
				closestDistanceSquared = distanceSquare;
				contactPoint = vertex;
			}
		}
	}
}
bool CollideBalls(Shape& firstShape, Shape& secondShape, Collision& collision)
{
	Vector2 centerOffset = secondShape.position - firstShape.position;
	double depth = firstShape.radius + secondShape.radius - centerOffset.GetLength();
	if (depth < 0) { return false; }
	collision.depth = depth;
	collision.normal = centerOffset.IsZero() ? Vector2(0, 1) : centerOffset.GetNormalized();
	if (firstShape.radius < secondShape.radius)
	{
		collision.contactPoint = firstShape.position + firstShape.radius * collision.normal;
	}
	else
	{
		collision.contactPoint = secondShape.position - secondShape.radius * collision.normal;
	} return true;
}
double CalculateBallCollisionDepth(Vector2 normal, Vector2 origin, Shape const& shape, Vector2& contactPoint)
{
	auto tmp = Dot(shape.position - origin, normal) - shape.radius;
	contactPoint = shape.position - shape.radius * normal; return -tmp;
}
double CalculatePolygonCollisionDepth(Vector2 normal, Vector2 origin, Shape const& shape, Vector2& contactPoint)
{
	double minimumProjection = DBL_MAX;
	for (auto vertex : shape.vertices)
	{
		auto tmp = Dot(vertex - origin, normal);
		if (tmp < minimumProjection)
		{
			minimumProjection = tmp; contactPoint = vertex;
		}
	} return -minimumProjection;
}



