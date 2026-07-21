#include "Collision.h"
#include "SimulationApp.h"
#include "Background.h"
#include "Body.h"
#include "Shape.h"
#include "DrawGeometry.h"

//	疑似废弃
void Collision::Render(SimulationApp& cur) const
{
	drawEllipse(cur.scr, cur.dscr, 100, c, 5, 5, cur.background->ViewPort(), dColor::YELLOW);
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

	// c 点处两个物体的切向量（tangent vector）和对应速度
	Vector2 tanC1 = c - body0->position;
	Vector2 tanC2 = c - body1->position;
	tanC1 = Vector2(-tanC1.y, tanC1.x);//逆时针转90度
	tanC2 = Vector2(-tanC2.y, tanC2.x);
	Vector2 velocity0 = body0->velocity + body0->velocityAngular * tanC1;
	Vector2 velocity1 = body1->velocity + body1->velocityAngular * tanC2;

	// 法线方向
	// @todo：这里需要补物理知识
	double tanC0normal = dot(tanC1, n);
	double tanC1normal = dot(tanC2, n);
	double inverse_i0 = body0->invMass + tanC0normal * tanC0normal * body0->invInertia;
	double inverse_i1 = body1->invMass + tanC1normal * tanC1normal * body1->invInertia;
	double impulseNormal = (1 + elasticity) * max(0.0, dot(velocity0 - velocity1, n)) / (inverse_i0 + inverse_i1); // 法向方向的冲量

	Vector2 impulse;
	impulse = -impulseNormal * n;
	body0->velocity += impulse * body0->invMass;
	body1->velocity -= impulse * body1->invMass;
	body0->velocityAngular += dot(impulse, tanC1) * body0->invInertia;
	body1->velocityAngular -= dot(impulse, tanC2) * body1->invInertia;

	Vector2 updatedVelocity0 = body0->velocity + body0->velocityAngular * tanC1;
	Vector2 updatedVelocity1 = body1->velocity + body1->velocityAngular * tanC2;

	double staticFrictionImpulse = frictionStatic * impulseNormal;
	double dynamicFrictionImpulse = frictionDynamic * impulseNormal;


	// 切线方向
	Vector2 tangentDirection = Vector2(-n.y, n.x);
	double impulseTangential = dot(updatedVelocity0 - updatedVelocity1, tangentDirection);
	if (impulseTangential < 0)
	{
		impulseTangential = -impulseTangential; tangentDirection = -tangentDirection;
	}
	double tanCtan0 = dot(tanC1, tangentDirection);// tantan？？？
	double tanCtan1 = dot(tanC2, tangentDirection);
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
	body0->velocityAngular += dot(impulse, tanC1) * body0->invInertia;
	body1->velocityAngular -= dot(impulse, tanC2) * body1->invInertia;

	if (equalResponse)
	{
		if (body0->invMass == 0) { body1->position += n * d; }
		else if (body1->invMass == 0) { body0->position -= n * d; }
		else
		{
			body0->position -= n * d / 2;
			body1->position += n * d / 2;
		}
		return;
	}
	// 我是真的不知道这个有没有道理。
	double sum = body0->invMass + body1->invMass;
	body0->position -= n * d * body0->invMass / sum;
	body1->position += n * d * body1->invMass / sum;
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
				if (!collide(*shape0, *shape1, collision, false)) { continue; }
				if (!collide(*shape1, *shape0, collision, true)) { continue; }
			}


			// 都不是圆，检查最近距离是否小于阈值
			if (!shape0->isCircle && !shape1->isCircle && collision.diff < parallelEpsilon)
			{
				double distanceSquareClosest = DBL_MAX;
				FindClosestContactVertex(*shape0, *shape1, collision.c, distanceSquareClosest);
				FindClosestContactVertex(*shape1, *shape0, collision.c, distanceSquareClosest);
			}

			out.push_back(std::make_shared<Collision>(collision));
		}
}

/***need be move to private↓***/

bool collide(Shape& sh0, Shape& sh1, Collision& out, bool reverse)
{
	Vector2 c;
	if (sh0.isCircle)
	{
		for (int i = (0), iEnd = int(sh1.vertices.size()); i < iEnd; ++i)
		{
			Vector2 n = (sh1.vertices[i] - sh0.position).normalize();
			if (n.zero()) { n = Vector2(0, 1); }
			auto d = CalculatePolygonCollisionDepth(n, sh0.position + n * sh0.r, sh1, c);
			if (d < 0) { return false; }
			if (d < out.d)
			{
				out.diff = out.d - d;
				out.d = d;
				out.c = c;
				out.n = reverse ? -n : n;
			}
		}
		return true;
	}

	for (int i = (0), iEnd = int(sh0.vertices.size()); i < iEnd; ++i)
	{
		int j = (i + 1) % sh0.vertices.size();
		Vector2 n = sh0.vertices[j] - sh0.vertices[i];
		n = Vector2(n.y, -n.x).normalize();
		auto d = sh1.isCircle ?
			CalculateBallCollisionDepth(n, sh0.vertices[i], sh1, c) :
			CalculatePolygonCollisionDepth(n, sh0.vertices[i], sh1, c);
		if (d < 0) { return false; }
		if (d < out.d)
		{
			out.diff = out.d - d;
			out.d = d;
			out.c = c;
			out.n = reverse ? -n : n;
		}
	} return true;
}
void FindClosestContactVertex(Shape& shape0, Shape& shape1, Vector2& contactP, double& closestDistanceSqr)
{
	for (int i = (0), iEnd = int(shape0.vertices.size()); i < iEnd; ++i)
	{
		int j = (i + 1) % shape0.vertices.size();
		for (auto v : shape1.vertices)
		{
			//算出该顶点到 shape0 当前顶点和下一个顶点所构成的线段的距离平方
			double distanceSquare = GetDistanceSquare(shape0.vertices[i], shape0.vertices[j], v);
			if (distanceSquare < closestDistanceSqr)
			{
				closestDistanceSqr = distanceSquare;
				contactP = v;
			}
		}
	}
}
bool CollideBalls(Shape& sh0, Shape& sh1, Collision& out)
{
	Vector2 o10 = sh1.position - sh0.position;
	double d = sh0.r + sh1.r - o10.len();
	if (d < 0) { return false; }
	out.d = d;
	out.n = o10.zero() ? Vector2(0, 1) : o10.normalize();
	if (sh0.r < sh1.r)
	{
		out.c = sh0.position + sh0.r * out.n;
	}
	else
	{
		out.c = sh1.position - sh1.r * out.n;
	} return true;
}
double CalculateBallCollisionDepth(Vector2 n, Vector2 o, Shape const& sh, Vector2& c)
{
	auto tmp = dot(sh.position - o, n) - sh.r;
	c = sh.position - sh.r * n; return -tmp;
}
double CalculatePolygonCollisionDepth(Vector2 n, Vector2 o, Shape const& sh, Vector2& c)
{
	double out = DBL_MAX;
	for (auto v : sh.vertices)
	{
		auto tmp = dot(v - o, n);
		if (tmp < out)
		{
			out = tmp; c = v;
		}
	} return -out;
}



