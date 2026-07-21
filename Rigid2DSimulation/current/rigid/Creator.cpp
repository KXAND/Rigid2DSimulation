#include "Creator.h"
#include "current/common/utils.h"
#include "SimulationApp.h"
#include "Background.h"
#include "Value.h"
#include "Body.h"
#include "Shape.h"
#include "Connection.h"

Creator::Creator()
{
	DepthBody = 10;
	DepthConnection = -1;
	thick = 10;
	rad = 8;
}
Creator::Creator(Value const& v) : Creator()
{
	if (auto md = FindConfigValue(v, L"mode"))
	{
		if (md->typ == L"str")
		{
			auto s = md->str;
			if (s == L"box") { mode = CREATE_BOX; }
			else if (s == L"ball") { mode = CREATE_BALL; }
			else if (s == L"plate") { mode = CREATE_PLATE; }
			else if (s == L"connection") { mode = CREATE_CONN; }
			else if (s == L"point") { mode = CREATE_POINT; }
			else if (s == L"nail") { mode = CREATE_NAIL; }
			else if (s == L"particle") { mode = CREATE_PARTICLE; }
		}
		else { mode = md->num; }
	}
	TryReadConfigNumber(v, L"thickness", thick);
	TryReadConfigNumber(v, L"radius", rad);
	if (auto value = FindConfigValue(v, L"bodyConfiguration")) { bodyConfiguration = std::make_shared<Value>(*value); }
	if (auto value = FindConfigValue(v, L"connectionConfiguration")) { connectionConfiguration = std::make_shared<Value>(*value); }
}

double Creator::Depth() const
{
	switch (mode)
	{
		case CREATE_CONN:
		case CREATE_NAIL:
			return DepthConnection;
		default:
			break;
	}
	return DepthBody;
}

void Creator::UpdateBox(SimulationApp& cur)
{
	if (active)
	{
		double x0 = creationStartPoint.x, y0 = creationStartPoint.y;
		double x1 = cur.input.mousePosition.x, y1 = cur.input.mousePosition.y;
		double min_wh = 10;
		if (x1 - x0 >= min_wh && y1 - y0 >= min_wh)
		{
			body = std::make_shared<Body>(std::make_shared<Shape>(GetRectVerticesByCenter(x1 - x0, y1 - y0)));
			body->position = Vector2(x0 + x1, y0 + y1) / 2;
			body->innerColor = Color(GenerateRandomInt(256), GenerateRandomInt(256), GenerateRandomInt(256));
			if (bodyConfiguration) { body->ReadCfg(*bodyConfiguration); }
			body->Init();
		}

		if (!cur.input.mouseButtonDown[0])
		{
			active = false;
			if (body)
			{
				cur.bodies.push_back(body);
				body = NULL; cur.isSceneChanged = true;
			}
		}
	}
	else
	{
		if (hovered && cur.input.IsMouseClickedDown(0))
		{
			active = true; creationStartPoint = cur.input.mousePosition;
		}
	}
}
void Creator::UpdateBall(SimulationApp& cur)
{
	if (active)
	{
		double r = sqrt((creationStartPoint - cur.input.mousePosition).lensqr());
		double min_r = 7.5;
		if (r > min_r)
		{
			body = std::make_shared<Body>(std::make_shared<Shape>(r));
			body->position = Vector2(creationStartPoint);
			body->innerColor = Color(GenerateRandomInt(256), GenerateRandomInt(256), GenerateRandomInt(256));
			if (bodyConfiguration) { body->ReadCfg(*bodyConfiguration); }
			body->Init();
		}

		if (!cur.input.mouseButtonDown[0])
		{
			active = false;
			if (body)
			{
				cur.bodies.push_back(body);
				body = NULL; cur.isSceneChanged = true;
			}
		}
	}
	else
	{
		if (hovered && cur.input.IsMouseClickedDown(0))
		{
			active = true; creationStartPoint = cur.input.mousePosition;
		}
	}
}
void Creator::UpdatePlate(SimulationApp& cur)
{
	if (active)
	{
		double thk = thick;
		double min_len = 5;
		DVector2 v = creationStartPoint - cur.input.mousePosition;
		double len = sqrt(v.lensqr());
		if (len > min_len)
		{
			body = std::make_shared<Body>(std::make_shared<Shape>(GetRectVerticesByCenter(len + thk, thk)));
			body->position = Vector2(creationStartPoint + cur.input.mousePosition) / 2;
			body->radian = atan2(v.y, v.x);
			body->innerColor = Color(GenerateRandomInt(256), GenerateRandomInt(256), GenerateRandomInt(256));
			if (bodyConfiguration) { body->ReadCfg(*bodyConfiguration); }
			body->Init();
		}

		if (!cur.input.mouseButtonDown[0])
		{
			active = false;
			if (body)
			{
				cur.bodies.push_back(body);
				body = NULL; cur.isSceneChanged = true;
			}
		}
	}
	else
	{
		if (hovered && cur.input.IsMouseClickedDown(0))
		{
			active = true; creationStartPoint = cur.input.mousePosition;
		}
	}
}
void Creator::UpdateConnection(SimulationApp& cur)
{
	if (active)
	{
		connection = std::make_shared<Connection>();
		if (connectionConfiguration) { connection->ReadCfg(*connectionConfiguration); }
		// 要求 creator 的更新在 body 后面对吧，是不是耦合太高了。
		// 要求 creator 的更新在 body 后面对吧，是不是耦合太高了。
		if (!cur.input.mouseButtonDown[0])
		{
			if (body1)
			{
				connection->body0 = body0;
				connection->body1 = body1;
				connection->p0Relative = p0Relative;
				connection->p1Relative = p1Relative;

				connection->SignUpToBodies();
				connection->UpdatePosition();
				connection->len = (connection->p0 - connection->p1).len();
				cur.connections.push_back(connection);
				cur.isSceneChanged = true;
			}
			connection = NULL;
			body0 = body1 = NULL; active = false;
		}
	}
	else
	{
		if (body0) { active = true; }
	}
}
void Creator::UpdatePoint(SimulationApp& cur)
{
	if (hovered && cur.input.IsMouseClickedDown(0))
	{
		body = std::make_shared<Body>((Vector2) cur.input.mousePosition);
		body->innerColor = Color(GenerateRandomInt(256), GenerateRandomInt(256), GenerateRandomInt(256));
		if (bodyConfiguration) { body->ReadCfg(*bodyConfiguration); }
		body->Init();
		cur.bodies.push_back(body);
		body = NULL; 
		cur.isSceneChanged = true;
	}
}
void Creator::UpdateNail(SimulationApp& cur)
{
	if (body0)
	{
		body = std::make_shared<Body>(body0->position + body0->transform * p0Relative);
		body->Init();
		cur.bodies.push_back(body);
		connection = std::make_shared<Connection>();
		connection->type = CON_LINK;
		connection->body0 = body0;
		connection->body1 = &*body;
		connection->p0Relative = p0Relative;
		connection->SignUpToBodies();
		connection->UpdatePosition();
		cur.connections.push_back(connection);

		body0 = NULL; connection = NULL;
		body = NULL; cur.isSceneChanged = true;
	}
}
void Creator::UpdateParticle(SimulationApp& cur)
{
	if (hovered && cur.input.IsMouseClickedDown(0))
	{
		body = std::make_shared<Body>(std::make_shared<Shape>(rad));
		body->position = (Vector2) cur.input.mousePosition;
		body->innerColor = Color(GenerateRandomInt(256), GenerateRandomInt(256), GenerateRandomInt(256));
		if (bodyConfiguration) { body->ReadCfg(*bodyConfiguration); }
		body->Init();
		cur.bodies.push_back(body);
		body = NULL; cur.isSceneChanged = true;
	}
}
void Creator::Update(SimulationApp& cur)
{
	hovered = (cur.own.hovered == this);
	body = NULL; connection = NULL;

	switch (mode)
	{
		case CREATE_BOX: UpdateBox(cur); break;
		case CREATE_BALL: UpdateBall(cur); break;
		case CREATE_PLATE: UpdatePlate(cur); break;
		case CREATE_CONN: UpdateConnection(cur); break;
		case CREATE_POINT: UpdatePoint(cur); break;
		case CREATE_NAIL: UpdateNail(cur); break;
		case CREATE_PARTICLE: UpdateParticle(cur); break;
	}


	if (body) { body->Render(cur); }
	if (connection && body0)
	{
		connection->p0 = body0->position + body0->transform * p0Relative;
		connection->p1 = (Vector2) cur.input.mousePosition;
		connection->Render(cur);
	}
}
void Creator::Discard(SimulationApp& cur)
{
	cur.own.Remove(this); body0 = body1 = NULL;
	body = NULL; connection = NULL;
	hovered = active = false;
}
void Creator::PreUpdate(SimulationApp& cur)
{
	if (cur.own.hoveredDepth <= Depth() && isInside(cur.input.mousePosition, cur.background->ViewPort()))
	{
		cur.own.hoveredDepth = Depth();
		cur.own.hovered = this;
	}
}



