#include "Connection.h"
#include "Body.h"
#include "current/common/utils.h"
#include "SimulationApp.h"
#include "DrawLineSegment.h"
#include "Background.h"
#include "Connection.h"

Connection::Connection()
{
	colorRope = Color::WHITE;
	colorRopeTight = Color::RED;
	colorLink = Color::BLUE;
	colorSpringCompress = Color::GREEN;
	colorSpringStretch = Color::YELLOW;
	colorCord = Color::CYAN;
	colorCordTight = Color::MAGENTA;
}
void Connection::Save(SimulationApp const& simulationApp, FILE* file) const
{
	int size = 0;
	fwrite(&(type), sizeof(type), 1, file);
	size = (scriptText).size();
	fwrite(&(size), sizeof(size), 1, file);
	fwrite(&*(scriptText).begin(), sizeof(*(&*(scriptText).begin())), size, file);
	auto pred0 = [this](std::shared_ptr<Body> b) { return &*b == body0; };
	int ind0 = find_if(simulationApp.bodies.begin(), simulationApp.bodies.end(), pred0) - simulationApp.bodies.begin();
	auto pred1 = [this](std::shared_ptr<Body> b) { return &*b == body1; };
	int ind1 = find_if(simulationApp.bodies.begin(), simulationApp.bodies.end(), pred1) - simulationApp.bodies.begin();
	fwrite(&(ind0), sizeof(ind0), 1, file);
	fwrite(&(ind1), sizeof(ind1), 1, file);
	fwrite(&(p0Relative), sizeof(p0Relative), 1, file);
	fwrite(&(p1Relative), sizeof(p1Relative), 1, file);
	fwrite(&(length), sizeof(length), 1, file);
	fwrite(&(springConstant), sizeof(springConstant), 1, file);
}
Connection::Connection(SimulationApp& simulationApp, FILE* file) : Connection()
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
	body0 = &*simulationApp.bodies[ind0]; body1 = &*simulationApp.bodies[ind1];
	fread(&(p0Relative), sizeof(p0Relative), 1, file);
	fread(&(p1Relative), sizeof(p1Relative), 1, file);
	fread(&(length), sizeof(length), 1, file);
	fread(&(springConstant), sizeof(springConstant), 1, file);
	RefreshFromScript(simulationApp); SignUpToBodies(); UpdatePosition();
}

void Connection::ReadConfig(Value const& configuration)
{
	TryReadConfigNumber(configuration, L"length", length);
	TryReadConfigNumber(configuration, L"springConstant", springConstant);
	if (auto md = FindConfigValue(configuration, L"type"))
	{
		if (md->type == L"str")
		{
			auto s = md->stringValue;
			if (s == L"rope") { type = kRopeConnection; }
			else if (s == L"link") { type = kLinkConnection; }
			else if (s == L"spring") { type = kSpringConnection; }
			else if (s == L"cord") { type = kCordConnection; }
		}
		else { type = md->number; }
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

void Connection::RefreshFromScript(SimulationApp& simulationApp)
{
	simulationApp.currentScope[L"connection"] = std::make_shared<Value>();
	Execute(simulationApp.currentScope, Compile(scriptText));
	if (simulationApp.currentScope.find(L"connection") == simulationApp.currentScope.end()) { return; }
	ReadConfig(*simulationApp.currentScope[L"connection"]);

	UpdatePosition();
}

void Connection::SimulateRopeOrLink(bool equalReponse)
{
	UpdatePosition();
	if (body0 == body1) { return; }
	if (body0->invMass == 0 && body1->invMass == 0) { return; }

	Vector2 r0 = p0 - body0->position;
	Vector2 r1 = p1 - body1->position;
	//逆时针旋转90度
	r0 = Vector2(-r0.y, r0.x);
	r1 = Vector2(-r1.y, r1.x);
	Vector2 v0 = body0->velocity + body0->velocityAngular * r0;
	Vector2 v1 = body1->velocity + body1->velocityAngular * r1;

	Vector2 d = p1 - p0;
	double currLenth = d.GetLength();
	d = d.GetNormalized();
	isTight = currLenth > length;
	if (!isTight && type == kRopeConnection) { return; }// rope在压缩时不受力，拉伸时和link一样不形变

	double torque0 = Dot(r0, d);
	double torque1 = Dot(r1, d);
	double accelFactor0 = body0->invMass + torque0 * torque0 * body0->invInertia;
	double accelFactor1 = body1->invMass + torque1 * torque1 * body1->invInertia;
	double velocityDiffOnD = Dot(v0 - v1, d);
	if (type == kRopeConnection) { velocityDiffOnD = min(0.0, velocityDiffOnD); }
	Vector2 impulse = -velocityDiffOnD / (accelFactor0 + accelFactor1) * d;

	body0->velocity += impulse * body0->invMass;
	body1->velocity -= impulse * body1->invMass;
	body0->velocityAngular += Dot(impulse, r0) * body0->invInertia;
	body1->velocityAngular -= Dot(impulse, r1) * body1->invInertia;

	if (equalReponse)
	{
		if (body0->invMass == 0)
		{
			body1->position += (currLenth - length) * d;
		}
		else if (body1->invMass == 0)
		{
			body0->position -= (currLenth - length) * d;
		}
		else
		{
			body0->position += (currLenth - length) * d / 2;
			body1->position -= (currLenth - length) * d / 2;
		}
		return;
	}
	body0->position += d * (currLenth - length) * body0->invMass / (body0->invMass + body1->invMass);
	body1->position -= d * (currLenth - length) * body1->invMass / (body0->invMass + body1->invMass);
}

void Connection::SimulateCordOrSpring(double deltaTime)
{
	UpdatePosition();
	if (body0 == body1) { return; }
	if (body0->invMass == 0 && body1->invMass == 0) { return; }

	Vector2 r0 = p0 - body0->position;
	Vector2 r1 = p1 - body1->position;
	//逆时针旋转90度
	r0 = Vector2(-r0.y, r0.x);
	r1 = Vector2(-r1.y, r1.x);
	Vector2 v0 = body0->velocity + body0->velocityAngular * r0;
	Vector2 v1 = body1->velocity + body1->velocityAngular * r1;

	Vector2 p0p1Vec = p1 - p0;
	double currLenth = p0p1Vec.GetLength();
	p0p1Vec = p0p1Vec.GetNormalized();
	if (p0p1Vec.IsZero()) { p0p1Vec = Vector2(1, 0); }

	isTight = currLenth > length;
	if (!isTight && type == kCordConnection) { return; }// cord在压缩不受力，拉伸和spring一样

	// @todo:关于力矩的公式我已经忘了，需要再推导
	double torgue0 = Dot(r0, p0p1Vec);
	double torgue1 = Dot(r1, p0p1Vec);
	Vector2 springsForce = (currLenth - length) * springConstant * deltaTime * p0p1Vec;

	// 每 dt内计算一次，可以直接加上加速度
	body0->velocity += springsForce * body0->invMass;
	body1->velocity -= springsForce * body1->invMass;
	body0->velocityAngular += Dot(springsForce, r0) * body0->invInertia;
	body1->velocityAngular -= Dot(springsForce, r1) * body1->invInertia;
}

void Connection::Simulate(double deltaTime, bool useEqualPositionCorrection)
{
	switch (type)
	{
		case kRopeConnection:
		case kLinkConnection:
			SimulateRopeOrLink(useEqualPositionCorrection);
			return;
		case kCordConnection:
		case kSpringConnection:
			SimulateCordOrSpring(deltaTime);
			return;
	}
}

void Connection::Render(SimulationApp& simulationApp) const
{
	Color color;
	switch (type)
	{
		case kRopeConnection: color = isTight ? colorRopeTight : colorRope; break;
		case kLinkConnection: color = colorLink; break;
		case kCordConnection: color = isTight ? colorCordTight : colorCord; break;
		case kSpringConnection: color = isTight ? colorSpringStretch : colorSpringCompress; break;
	}
	DrawLineSegment(simulationApp.screen, simulationApp.depthBuffer, p0, p1, 2, simulationApp.background->GetViewport(), color);
}

Body* GetOtherBody(Body* body, Connection const& connection)
{
	if (connection.body0 == body && connection.body1 == body)
	{
		return NULL;
	}
	if (connection.body0 != body && connection.body1 != body)
	{
		return NULL;
	}
	return connection.body0 == body ? connection.body1 : connection.body0;
}



