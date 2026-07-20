#include "connection.h"
#include "body.h"
#include "current/common/utils.h"
#include "simulation_app.h" 
#include "draw_px_seg.h"
#include "background.h"
#include "connection.h"

Connection::Connection()
{
	colorRope = dColor::WHITE;
	colorRopeTight = dColor::RED;
	colorLink = dColor::BLUE;
	colorSpringCompress = dColor::GREEN;
	colorSpringStretch = dColor::YELLOW;
	colorCord = dColor::CYAN;
	colorCordTight = dColor::MAGENTA;
}
void Connection::Save(SimulationApp const& cur, FILE* file) const
{
	int size = 0;
	fwrite(&(type), sizeof(type), 1, file);
	size = (scriptText).size();
	fwrite(&(size), sizeof(size), 1, file);
	fwrite(&*(scriptText).begin(), sizeof(*(&*(scriptText).begin())), size, file);
	auto pred0 = [this](std::shared_ptr<Body> b) { return &*b == body0; };
	int ind0 = find_if(cur.bodies.begin(), cur.bodies.end(), pred0) - cur.bodies.begin();
	auto pred1 = [this](std::shared_ptr<Body> b) { return &*b == body1; };
	int ind1 = find_if(cur.bodies.begin(), cur.bodies.end(), pred1) - cur.bodies.begin();
	fwrite(&(ind0), sizeof(ind0), 1, file);
	fwrite(&(ind1), sizeof(ind1), 1, file);
	fwrite(&(p0Relative), sizeof(p0Relative), 1, file);
	fwrite(&(p1Relative), sizeof(p1Relative), 1, file);
	fwrite(&(len), sizeof(len), 1, file);
	fwrite(&(hooke), sizeof(hooke), 1, file);
}
Connection::Connection(SimulationApp& cur, FILE* file) : Connection()
{
	int size = 0;
	fread(&(type), sizeof(type), 1, file);
	fread(&(size), sizeof(size), 1, file);
	(scriptText).resize(size);
	fread(&*(scriptText).begin(), sizeof(*(&*(scriptText).begin())), size, file);
	temporaryScriptText = scriptText;
	int ind0 = 0, ind1 = 0;
	fread(&(ind0), sizeof(ind0), 1, file);
	fread(&(ind1), sizeof(ind1), 1, file);
	body0 = &*cur.bodies[ind0]; body1 = &*cur.bodies[ind1];
	fread(&(p0Relative), sizeof(p0Relative), 1, file);
	fread(&(p1Relative), sizeof(p1Relative), 1, file);
	fread(&(len), sizeof(len), 1, file);
	fread(&(hooke), sizeof(hooke), 1, file);
	RefreshFromScript(cur); SignUpToBodies(); UpdatePosition();
}

void Connection::ReadCfg(Var const& cfg)
{
	TryReadConfigNumber(cfg, L"len", len);
	TryReadConfigNumber(cfg, L"hooke", hooke);
	if (auto md = FindConfigValue(cfg, L"type"))
	{
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
void Connection::SignUpToBodies()
{
	body0->connections.push_back(this);
	if (body0 != body1) { body1->connections.push_back(this); }
}

void Connection::UpdatePosition()
{
	p0 = body0->position + body0->transform * p0Relative;
	p1 = body1->position + body1->transform * p1Relative;
}

void Connection::RefreshFromScript(SimulationApp& cur)
{
	cur.currentScope[L"connection"] = std::make_shared<Var>();
	Execute(cur.currentScope, Compile(scriptText));
	if (cur.currentScope.find(L"connection") == cur.currentScope.end()) { return; }
	ReadCfg(*cur.currentScope[L"connection"]);

	UpdatePosition();
}

void Connection::SimulateRopeOrLink(bool equalReponse)
{
	UpdatePosition();
	if (body0 == body1) { return; }
	if (body0->invMass == 0 && body1->invMass == 0) { return; }

	vector2 r0 = p0 - body0->position;
	vector2 r1 = p1 - body1->position;
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
			body1->position += (currLenth - len) * d;
		}
		else if (body1->invMass == 0)
		{
			body0->position -= (currLenth - len) * d;
		}
		else
		{
			body0->position += (currLenth - len) * d / 2;
			body1->position -= (currLenth - len) * d / 2;
		}
		return;
	}
	body0->position += d * (currLenth - len) * body0->invMass / (body0->invMass + body1->invMass);
	body1->position -= d * (currLenth - len) * body1->invMass / (body0->invMass + body1->invMass);
}

void Connection::SimulateCordOrSpring(double deltaTime)
{
	UpdatePosition();
	if (body0 == body1) { return; }
	if (body0->invMass == 0 && body1->invMass == 0) { return; }

	vector2 r0 = p0 - body0->position;
	vector2 r1 = p1 - body1->position;
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

void Connection::Simulate(double deltaTime, bool useEqualPositionCorrection)
{
	switch (type)
	{
		case CON_ROPE:
		case CON_LINK:
			SimulateRopeOrLink(useEqualPositionCorrection);
			return;
		case CON_CORD:
		case CON_SPRING:
			SimulateCordOrSpring(deltaTime);
			return;
	}
}

void Connection::Render(SimulationApp& cur) const
{
	dColor color;
	switch (type)
	{
		case CON_ROPE: color = isTight ? colorRopeTight : colorRope; break;
		case CON_LINK: color = colorLink; break;
		case CON_CORD: color = isTight ? colorCordTight : colorCord; break;
		case CON_SPRING: color = isTight ? colorSpringStretch : colorSpringCompress; break;
	}
	drawLineSegment(cur.scr, cur.dscr, p0, p1, 2, cur.background->ViewPort(), color);
}

Body* GetOtherBody(Body* b, Connection const& con)
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



