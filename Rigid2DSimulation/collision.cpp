#include "collision.h"
#include "cur.h"
#include "bgr.h"
#include "body.h"
#include "shape.h"
#include "draw_geo.h"

#include "my_def.h"
//	���Ʒ���
void Collision::Render(Cur& cur) const
{
	drawEllipse(scr, dscr, 100, c, 5, 5, bgr.viewPort(), dColor::YELLOW);
}
void Collision::simulate(bool equalResponse)
{
	/*����Լ����
		��ײ�㣺c
		���壺1��2
	*/

	double elasticity = min(body0->elasticity, body1->elasticity);
	double frictionDynamic = min(body0->frictionDynamic, body1->frictionDynamic);
	double frictionStatic = min(body0->frictionStatic, body1->frictionStatic);

	// c �㴦�����������������tangent vector���Ͷ�Ӧ�ٶ�
	vector2 tanC1 = c - body0->o;
	vector2 tanC2 = c - body1->o;
	tanC1 = vector2(-tanC1.y, tanC1.x);//��ʱ��ת90��
	tanC2 = vector2(-tanC2.y, tanC2.x);
	vector2 velocity0 = body0->velocity + body0->velocityAngular * tanC1;
	vector2 velocity1 = body1->velocity + body1->velocityAngular * tanC2;

	// ���߷���
	// @todo��������Ҫ������֪ʶ
	double tanC0normal = dot(tanC1, n);
	double tanC1normal = dot(tanC2, n);
	double inverse_i0 = body0->invMass + tanC0normal * tanC0normal * body0->invInertia;
	double inverse_i1 = body1->invMass + tanC1normal * tanC1normal * body1->invInertia;
	double impulseNormal = (1 + elasticity) * max(0.0, dot(velocity0 - velocity1, n)) / (inverse_i0 + inverse_i1); // ������ĳ���

	vector2 impulse;
	impulse = -impulseNormal * n;
	body0->velocity += impulse * body0->invMass;
	body1->velocity -= impulse * body1->invMass;
	body0->velocityAngular += dot(impulse, tanC1) * body0->invInertia;
	body1->velocityAngular -= dot(impulse, tanC2) * body1->invInertia;

	vector2 updatedVelocity0 = body0->velocity + body0->velocityAngular * tanC1;
	vector2 updatedVelocity1 = body1->velocity + body1->velocityAngular * tanC2;

	double staticFrictionImpulse = frictionStatic * impulseNormal;
	double dynamicFrictionImpulse = frictionDynamic * impulseNormal;


	// ���߷���
	vector2 tangentDirection = vector2(-n.y, n.x);
	double impulseTangential = dot(updatedVelocity0 - updatedVelocity1, tangentDirection);
	if (impulseTangential < 0)
	{
		impulseTangential = -impulseTangential; tangentDirection = -tangentDirection;
	}
	double tanCtan0 = dot(tanC1, tangentDirection);// tantan������
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
		if (body0->invMass == 0) { body1->o += n * d; }
		else if (body1->invMass == 0) { body0->o -= n * d; }
		else
		{
			body0->o -= n * d / 2;
			body1->o += n * d / 2;
		}
		return;
	}
	// ������Ĳ�֪�������û�е���
	double sum = body0->invMass + body1->invMass;
	body0->o -= n * d * body0->invMass / sum;
	body1->o += n * d * body1->invMass / sum;
}

void Collide(Body& b0, Body& b1, vector<ptr<Collision>>& out, double eps_paralell)
{
	// �����ܷ�����ײ�����
	if (b0.invMass == 0 && b1.invMass == 0) { return; }
	if (!isOverlap(b0.box, b1.box)) { return; }

	for (auto& shape0 : b0.shapes)
		for (auto& shape1 : b1.shapes)
		{
			Collision collision;

			collision.body0 = &b0; collision.body1 = &b1;
			if (shape0->isCircle && shape1->isCircle)
			{
				// ������
				if (!collide_balls(*shape0, *shape1, collision)) { continue; }
			}
			else
			{
				if (!collide(*shape0, *shape1, collision, false)) { continue; }
				if (!collide(*shape1, *shape0, collision, true)) { continue; }
			}

			// ����һ����Բʱ��Ѱ������ĽӴ���
			if (!shape0->isCircle && !shape1->isCircle && collision.diff < eps_paralell)
			{
				double distanceSquareClosest = DBL_MAX;
				findClosestContactVertex(*shape0, *shape1, collision.c, distanceSquareClosest);
				findClosestContactVertex(*shape1, *shape0, collision.c, distanceSquareClosest);
			}

			out.push_back(msh<Collision>(collision));
		}
}

/***need be move to private��***/

bool collide(Shape& sh0, Shape& sh1, Collision& out, bool reverse)
{
	vector2 c;
	if (sh0.isCircle)
	{
		rep(i, 0, sh1.vertices.size())
		{
			vector2 n = (sh1.vertices[i] - sh0.o).normalize();
			if (n.zero()) { n = vector2(0, 1); }
			auto d = sub_collide_poly(n, sh0.o + n * sh0.r, sh1, c);
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

	rep(i, 0, sh0.vertices.size())
	{
		int j = (i + 1) % sh0.vertices.size();
		vector2 n = sh0.vertices[j] - sh0.vertices[i];
		n = vector2(n.y, -n.x).normalize();
		auto d = sh1.isCircle ?
			sub_collide_ball(n, sh0.vertices[i], sh1, c) :
			sub_collide_poly(n, sh0.vertices[i], sh1, c);
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
void findClosestContactVertex(Shape& shape0, Shape& shape1, vector2& contactP, double& closestDistanceSqr)
{
	rep(i, 0, shape0.vertices.size())
	{
		int j = (i + 1) % shape0.vertices.size();
		for (auto v : shape1.vertices)
		{
			//����ö��㵽 shape0 ��ǰ�������һ�����������ɵ��߶εľ���ƽ��
			double distanceSquare = getDistanceSquare(shape0.vertices[i], shape0.vertices[j], v);
			if (distanceSquare < closestDistanceSqr)
			{
				closestDistanceSqr = distanceSquare;
				contactP = v;
			}
		}
	}
}
bool collide_balls(Shape& sh0, Shape& sh1, Collision& out)
{
	vector2 o10 = sh1.o - sh0.o;
	double d = sh0.r + sh1.r - o10.len();
	if (d < 0) { return false; }
	out.d = d;
	out.n = o10.zero() ? vector2(0, 1) : o10.normalize();
	if (sh0.r < sh1.r)
	{
		out.c = sh0.o + sh0.r * out.n;
	}
	else
	{
		out.c = sh1.o - sh1.r * out.n;
	} return true;
}
double sub_collide_ball(vector2 n, vector2 o, Shape const& sh, vector2& c)
{
	auto tmp = dot(sh.o - o, n) - sh.r;
	c = sh.o - sh.r * n; return -tmp;
}
double sub_collide_poly(vector2 n, vector2 o, Shape const& sh, vector2& c)
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
