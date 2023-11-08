#include "connection.h"
#include "body.h"
#include "cur.h" 
#include "draw_px_seg.h"
#include "bgr.h"
#include "connection.h"

#include "my_def.h"
#define found(s) (dic.find(s) != dic.end())
#define getv(nm) if (found(L#nm)) { nm = dic.at(L#nm)->num; }

Connection::Connection()
{
	colorRope = dColor::BLACK;
	colorRopeTight = dColor::RED;
	colorLink = dColor::BLUE;
	colorSpringCompress = dColor::GREEN;
	colorSpringStretch = dColor::YELLOW;
	colorCord = dColor::CYAN;
	colorCordTight = dColor::MAGENTA;
}
void Connection::save(Cur const& cur, FILE* f) const
{
	int sz = 0;
	fwt(type); fwtv(cmd);
	auto pred0 = [this](ptr<Body> b) { return &*b == body0; };
	int ind0 = find_if(cur.bodies.begin(), cur.bodies.end(), pred0) - cur.bodies.begin();
	auto pred1 = [this](ptr<Body> b) { return &*b == body1; };
	int ind1 = find_if(cur.bodies.begin(), cur.bodies.end(), pred1) - cur.bodies.begin();
	fwt(ind0); fwt(ind1);
	fwt(p0Relative); fwt(p1Relative); fwt(len); fwt(hooke);
}
Connection::Connection(Cur& cur, FILE* f) : Connection()
{
	int sz = 0;
	frd(type); frdv(cmd); tmp_cmd = cmd;
	int ind0 = 0, ind1 = 0;
	frd(ind0); frd(ind1);
	body0 = &*cur.bodies[ind0]; body1 = &*cur.bodies[ind1];
	frd(p0Relative); frd(p1Relative); frd(len); frd(hooke);
	refresh(cur); signUpToBodies(); updatePosition();
}

void Connection::read_cfg(Var const& cfg)
{
	auto dic = cfg.dic;
	getv(len);  getv(hooke);
	if (found(L"type"))
	{
		auto md = dic.at(L"type");
		if (md->typ == L"str")
		{
			auto s = md->str;
			if (s == L"rope") { type = CON_ROPE; }
			else if (s == L"link") { type = CON_LINK; }
			else if (s == L"spring") { type = CON_SPRING; }
			else if (s == L"cord") { type = CON_CORD; }
		}
		else { type = md->num; }
	}
}
void Connection::signUpToBodies()
{
	body0->connections.push_back(this);
	if (body0 != body1) { body1->connections.push_back(this); }
}

void Connection::updatePosition()
{
	p0 = body0->o + body0->transform * p0Relative;
	p1 = body1->o + body1->transform * p1Relative;
}

void Connection::refresh(Cur& cur)
{
	gl[L"conn"] = msh<Var>();
	Execute(gl, Compile(cmd));
	if (gl.find(L"conn") == gl.end()) { return; }
	read_cfg(*gl[L"conn"]);

	updatePosition();
}

void Connection::simulateTypeRopeLink(bool equalReponse)
{
	updatePosition();
	if (body0 == body1) { return; }
	if (body0->invMass == 0 && body1->invMass == 0) { return; }

	vector2 r0 = p0 - body0->o;
	vector2 r1 = p1 - body1->o;
	//逆时针旋转90度
	r0 = vector2(-r0.y, r0.x);
	r1 = vector2(-r1.y, r1.x);
	vector2 v0 = body0->velocity + body0->velocityAngular * r0;
	vector2 v1 = body1->velocity + body1->velocityAngular * r1;

	vector2 d = p1 - p0;
	double currLenth = d.len();
	d = d.normalize();
	isTight = currLenth > len;
	if (!isTight && type == CON_ROPE) { return; }// rope在压缩时不受力，拉伸时和link一样不形变

	double torque0 = dot(r0, d);
	double torque1 = dot(r1, d);
	double accelFactor0 = body0->invMass + torque0 * torque0 * body0->invInertia;
	double accelFactor1 = body1->invMass + torque1 * torque1 * body1->invInertia;
	double velocityDiffOnD = dot(v0 - v1, d);
	if (type == CON_ROPE) { velocityDiffOnD = min(0.0, velocityDiffOnD); }
	vector2 impulse = -velocityDiffOnD / (accelFactor0 + accelFactor1) * d;

	body0->velocity += impulse * body0->invMass;
	body1->velocity -= impulse * body1->invMass;
	body0->velocityAngular += dot(impulse, r0) * body0->invInertia;
	body1->velocityAngular -= dot(impulse, r1) * body1->invInertia;

	if (equalReponse)
	{
		if (body0->invMass == 0)
		{
			body1->o += (currLenth - len) * d;
		}
		else if (body1->invMass == 0)
		{
			body0->o -= (currLenth - len) * d;
		}
		else
		{
			body0->o += (currLenth - len) * d / 2;
			body1->o -= (currLenth - len) * d / 2;
		}
		return;
	}
	body0->o += d * (currLenth - len) * body0->invMass / (body0->invMass + body1->invMass);
	body1->o -= d * (currLenth - len) * body1->invMass / (body0->invMass + body1->invMass);
}

void Connection::simulateTypeCordSpring(double deltaTime)
{
	updatePosition();
	if (body0 == body1) { return; }
	if (body0->invMass == 0 && body1->invMass == 0) { return; }

	vector2 r0 = p0 - body0->o;
	vector2 r1 = p1 - body1->o;
	//逆时针旋转90度
	r0 = vector2(-r0.y, r0.x);
	r1 = vector2(-r1.y, r1.x);
	vector2 v0 = body0->velocity + body0->velocityAngular * r0;
	vector2 v1 = body1->velocity + body1->velocityAngular * r1;

	vector2 p0p1Vec = p1 - p0;
	double currLenth = p0p1Vec.len();
	p0p1Vec = p0p1Vec.normalize();
	if (p0p1Vec.zero()) { p0p1Vec = vector2(1, 0); }

	isTight = currLenth > len;
	if (!isTight && type == CON_CORD) { return; }// cord在压缩不受力，拉伸和spring一样

	// @todo:关于力矩的公式我已经忘了，需要再推导
	double torgue0 = dot(r0, p0p1Vec);
	double torgue1 = dot(r1, p0p1Vec);
	vector2 springsForce = (currLenth - len) * hooke * deltaTime * p0p1Vec;

	// 每 dt内计算一次，可以直接加上加速度
	body0->velocity += springsForce * body0->invMass;
	body1->velocity -= springsForce * body1->invMass;
	body0->velocityAngular += dot(springsForce, r0) * body0->invInertia;
	body1->velocityAngular -= dot(springsForce, r1) * body1->invInertia;
}

void Connection::Simulate(double deltaTime, bool equal_repos)
{
	switch (type)
	{
		case CON_ROPE:
		case CON_LINK:
			simulateTypeRopeLink(equal_repos);
			return;
		case CON_CORD:
		case CON_SPRING:
			simulateTypeCordSpring(deltaTime);
			return;
	}
}

void Connection::Render(Cur& cur) const
{
	dColor color;
	switch (type)
	{
		case CON_ROPE: color = isTight ? colorRopeTight : colorRope; break;
		case CON_LINK: color = colorLink; break;
		case CON_CORD: color = isTight ? colorCordTight : colorCord; break;
		case CON_SPRING: color = isTight ? colorSpringStretch : colorSpringCompress; break;
	}
	drawLineSegment(scr, dscr, p0, p1, 2, bgr.viewPort(), color);
}

Body* getTheOtherBody(Body* b, Connection const& con)
{
	if (con.body0 == b && con.body1 == b)
	{
		return NULL;
	}
	if (con.body0 != b && con.body1 != b)
	{
		return NULL;
	}
	return con.body0 == b ? con.body1 : con.body0;
}
