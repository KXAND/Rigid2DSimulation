#pragma once
#include "vector2.h"

struct Cur;
struct Body;
struct Shape;
struct Collision {
	Body* b0 = NULL;
	Body* b1 = NULL;
	double d = DBL_MAX;
	double diff = DBL_MAX;
	vector2 n, c;
	void Render(Cur& cur) const;
	void Resolve(bool equal_repos);
};

void Collide(
	Body& b0, Body& b1, vector<ptr<Collision>>& out, double eps_paralell);
// ���� b0, b1 �Ѿ��������ˡ�
bool collide(Shape& sh0, Shape& sh1, Collision& out, bool reverse);
void find_contact(Shape& sh0, Shape& sh1, vector2& c, double &dsqr_close);
bool collide_balls(Shape& sh0, Shape& sh1, Collision& out);
double sub_collide_ball(vector2 n, vector2 o, Shape const& sh, vector2& c);
double sub_collide_poly(vector2 n, vector2 o, Shape const& sh, vector2& c);