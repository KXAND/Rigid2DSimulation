#include "creator.h"
#include "current/common/utils.h"
#include "simulation_app.h"
#include "background.h"
#include "var.h"
#include "body.h"
#include "shape.h"
#include "connection.h"

Creator::Creator()
{
	depthBody = 10;
	depthConnection = -1;
	thick = 10;
	rad = 8;
}
Creator::Creator(Var const& v) : Creator()
{
	if (auto md = FindConfigValue(v, L"mode"))
	{
		if (md->typ == L"str")
		{
			auto s = md->str;
			if (s == L"box") { mode = CREATE_BOX; }
			else if (s == L"ball") { mode = CREATE_BALL; }
			else if (s == L"plate") { mode = CREATE_PLATE; }
			else if (s == L"conn") { mode = CREATE_CONN; }
			else if (s == L"point") { mode = CREATE_POINT; }
			else if (s == L"nail") { mode = CREATE_NAIL; }
			else if (s == L"particle") { mode = CREATE_PARTICLE; }
		}
		else { mode = md->num; }
	}
	TryReadConfigNumber(v, L"thick", thick);
	TryReadConfigNumber(v, L"rad", rad);
	if (auto value = FindConfigValue(v, L"cfg_body")) { mkp(cfg_body)(*value); }
	if (auto value = FindConfigValue(v, L"cfg_conn")) { mkp(cfg_connection)(*value); }
}

double Creator::depth() const
{
	switch (mode)
	{
		case CREATE_CONN:
		case CREATE_NAIL:
			return depthConnection;
		default:
			break;
	}
	return depthBody;
}

void Creator::UpdateBox(SimulationApp& cur)
{
	if (active)
	{
		double x0 = p_rec.x, y0 = p_rec.y;
		double x1 = cur.input.msp.x, y1 = cur.input.msp.y;
		double min_wh = 10;
		if (x1 - x0 >= min_wh && y1 - y0 >= min_wh)
		{
			mkp(body)(msh<Shape>(GetRectVerticesByCenter(x1 - x0, y1 - y0)));
			body->position = vector2(x0 + x1, y0 + y1) / 2;
			body->innerColor = dColor(GenerateRandomInt(256), GenerateRandomInt(256), GenerateRandomInt(256));
			if (cfg_body) { body->ReadCfg(*cfg_body); }
			body->Init();
		}

		if (!cur.input.msd[0])
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
		if (hovered && cur.input.isMouseClickedDown(0))
		{
			active = true; p_rec = cur.input.msp;
		}
	}
}
void Creator::UpdateBall(SimulationApp& cur)
{
	if (active)
	{
		double r = sqrt((p_rec - cur.input.msp).lensqr());
		double min_r = 7.5;
		if (r > min_r)
		{
			mkp(body)(msh<Shape>(r));
			body->position = vector2(p_rec);
			body->innerColor = dColor(GenerateRandomInt(256), GenerateRandomInt(256), GenerateRandomInt(256));
			if (cfg_body) { body->ReadCfg(*cfg_body); }
			body->Init();
		}

		if (!cur.input.msd[0])
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
		if (hovered && cur.input.isMouseClickedDown(0))
		{
			active = true; p_rec = cur.input.msp;
		}
	}
}
void Creator::UpdatePlate(SimulationApp& cur)
{
	if (active)
	{
		double thk = thick;
		double min_len = 5;
		dVector2 v = p_rec - cur.input.msp;
		double len = sqrt(v.lensqr());
		if (len > min_len)
		{
			mkp(body)(msh<Shape>(GetRectVerticesByCenter(len + thk, thk)));
			body->position = vector2(p_rec + cur.input.msp) / 2;
			body->radian = atan2(v.y, v.x);
			body->innerColor = dColor(GenerateRandomInt(256), GenerateRandomInt(256), GenerateRandomInt(256));
			if (cfg_body) { body->ReadCfg(*cfg_body); }
			body->Init();
		}

		if (!cur.input.msd[0])
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
		if (hovered && cur.input.isMouseClickedDown(0))
		{
			active = true; p_rec = cur.input.msp;
		}
	}
}
void Creator::UpdateConnection(SimulationApp& cur)
{
	if (active)
	{
		mkp(connection)();
		if (cfg_connection) { connection->ReadCfg(*cfg_connection); }
		// 要求 creator 的更新在 body 后面对吧，是不是耦合太高了。
		// 要求 creator 的更新在 body 后面对吧，是不是耦合太高了。
		if (!cur.input.msd[0])
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
	if (hovered && cur.input.isMouseClickedDown(0))
	{
		mkp(body)((vector2) cur.input.msp);
		body->innerColor = dColor(GenerateRandomInt(256), GenerateRandomInt(256), GenerateRandomInt(256));
		if (cfg_body) { body->ReadCfg(*cfg_body); }
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
		mkp(body)(body0->position + body0->transform * p0Relative);
		body->Init();
		cur.bodies.push_back(body);
		mkp(connection)();
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
	if (hovered && cur.input.isMouseClickedDown(0))
	{
		mkp(body)(msh<Shape>(rad));
		body->position = (vector2) cur.input.msp;
		body->innerColor = dColor(GenerateRandomInt(256), GenerateRandomInt(256), GenerateRandomInt(256));
		if (cfg_body) { body->ReadCfg(*cfg_body); }
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
		connection->p1 = (vector2) cur.input.msp;
		connection->Render(cur);
	}
}
void Creator::Discard(SimulationApp& cur)
{
	cur.own.remove(this); body0 = body1 = NULL;
	body = NULL; connection = NULL;
	hovered = active = false;
}
void Creator::PreUpdate(SimulationApp& cur)
{
	if (cur.own.hoveredDepth <= depth() && isInside(cur.input.msp, cur.background->ViewPort()))
	{
		cur.own.hoveredDepth = depth();
		cur.own.hovered = this;
	}
}



