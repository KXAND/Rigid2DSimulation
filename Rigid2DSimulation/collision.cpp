#include "collision.h"
#include "cur.h"
#include "bgr.h"
#include "body.h"
#include "shape.h"
#include "draw_geo.h"

#include "my_def.h"

void Collision::Render(Cur& cur) const
{
	drawEllipse(scr, dscr, 100, c, 5, 5, bgr.viewPort(), dColor(255, 255, 0));
}
void Collision::simulate(bool equalResponse)
{
	double elasticity = min(b0->elasticity, b1->elasticity);
	double fDynamic = min(b0->frictionDynamic, b1->frictionDynamic);
	double fStatic = min(b0->frictionStatic, b1->frictionStatic);

	vector2 r0 = c - b0->o;
	vector2 r1 = c - b1->o;
	r0 = vector2(-r0.y, r0.x);//逆时针转90度
	r1 = vector2(-r1.y, r1.x);
	vector2 v0 = b0->velocity + b0->velocityAngular * r0;
	vector2 v1 = b1->velocity + b1->velocityAngular * r1;

	vector2 j;
	double rn0 = dot(r0, n);
	double rn1 = dot(r1, n);
	double inv_i0 = b0->invMass + rn0 * rn0 * b0->invInertia;
	double inv_i1 = b1->invMass + rn1 * rn1 * b1->invInertia;
	double jNormal = (1 + elasticity) * max(0.0, dot(v0 - v1, n)) / (inv_i0 + inv_i1); // 法向方向的冲量

	j = -jNormal * n;
	b0->velocity += j * b0->invMass;
	b1->velocity -= j * b1->invMass;
	b0->velocityAngular += dot(j, r0) * b0->invInertia;
	b1->velocityAngular -= dot(j, r1) * b1->invInertia;

	vector2 u0 = b0->velocity + b0->velocityAngular * r0;
	vector2 u1 = b1->velocity + b1->velocityAngular * r1;

	double js = fStatic * jNormal;
	double jd = fDynamic * jNormal;

	vector2 t = vector2(-n.y, n.x);
	double jTangential = dot(u0 - u1, t);
	if (jTangential < 0) { jTangential = -jTangential; t = -t; }
	double rt0 = dot(r0, t);
	double rt1 = dot(r1, t);
	double inv_it0 = b0->invMass + rt0 * rt0 * b0->invInertia;
	double inv_it1 = b1->invMass + rt1 * rt1 * b1->invInertia;
	jTangential /= inv_it0 + inv_it1;

	if (jTangential < js) { j = -jTangential * t; }
	else { j = -jd * t; }

	b0->velocity += j * b0->invMass;
	b1->velocity -= j * b1->invMass;
	b0->velocityAngular += dot(j, r0) * b0->invInertia;
	b1->velocityAngular -= dot(j, r1) * b1->invInertia;

	if (equalResponse)
	{
		if (b0->invMass == 0) { b1->o += n * d; }
		else if (b1->invMass == 0) { b0->o -= n * d; }
		else
		{
			b0->o -= n * d / 2;
			b1->o += n * d / 2;
		} return;
	}
	// 我是真的不知道这个有没有道理。
	double sum = b0->invMass + b1->invMass;
	b0->o -= n * d * b0->invMass / sum;
	b1->o += n * d * b1->invMass / sum;
}

void Collide(Body& b0, Body& b1, vector<ptr<Collision>>& out, double eps_paralell)
{
	if (b0.invMass == 0 && b1.invMass == 0) { return; }
	if (!isOverlap(b0.box, b1.box)) { return; }

	for (auto sh0 : b0.shapes) for (auto sh1 : b1.shapes)
	{
		Collision col;
		col.b0 = &b0; col.b1 = &b1;
		if (sh0->isCircle && sh1->isCircle)
		{
			if (!collide_balls(*sh0, *sh1, col)) { continue; }
		}
		else
		{
			if (!collide(*sh0, *sh1, col, false)) { continue; }
			if (!collide(*sh1, *sh0, col, true)) { continue; }
		}

		// 下面的小于号有一点重要。
		if (!sh0->isCircle && !sh1->isCircle && col.diff < eps_paralell)
		{
			double dsqr_close = DBL_MAX;
			find_contact(*sh0, *sh1, col.c, dsqr_close);
			find_contact(*sh1, *sh0, col.c, dsqr_close);
		}
		out.push_back(msh<Collision>(col));
	}
}
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
		} return true;
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
void find_contact(Shape& sh0, Shape& sh1, vector2& c, double& dsqr_close)
{
	rep(i, 0, sh0.vertices.size())
	{
		int j = (i + 1) % sh0.vertices.size();
		for (auto v : sh1.vertices)
		{
			double dsqr = dist_sqr(sh0.vertices[i], sh0.vertices[j], v);
			// 这里的小于号很重要。
			if (dsqr < dsqr_close)
			{
				dsqr_close = dsqr; c = v;
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
