#include "creator.h"
#include "cur.h"
#include "bgr.h"
#include "var.h"
#include "body.h"
#include "shape.h"
#include "connection.h"

#define found(s) (dic.find(s) != dic.end())
#define getv(nm) if (found(L#nm)) { nm = dic.at(L#nm)->num; }

Creator::Creator()
{
	dep_body = 10;
	dep_con = -1;
	thick = 10;
	rad = 8;
}
Creator::Creator(Var const& v) : Creator()
{
	auto dic = v.dic;
	if (found(L"mode"))
	{
		auto md = dic.at(L"mode");
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
	getv(thick); getv(rad);
	if (found(L"cfg_body")) { mkp(cfg_body)(*dic[L"cfg_body"]); }
	if (found(L"cfg_conn")) { mkp(cfg_connection)(*dic[L"cfg_conn"]); }
}

double Creator::dep() const
{
	switch (mode)
	{
		case CREATE_CONN:
		case CREATE_NAIL:
			return dep_con;
		default:
			break;
	}
	return dep_body;
}

void Creator::updateBox(Cur& cur)
{
	if (active)
	{
		double x0 = p_rec.x, y0 = p_rec.y;
		double x1 = cur.input.msp.x, y1 = cur.input.msp.y;
		double min_wh = 10;
		if (x1 - x0 >= min_wh && y1 - y0 >= min_wh)
		{
			mkp(body)(msh<Shape>(getRectVerticesByCenter(x1 - x0, y1 - y0)));
			body->o = vector2(x0 + x1, y0 + y1) / 2;
			body->innerColor = dColor(generateRadomInt(256), generateRadomInt(256), generateRadomInt(256));
			if (cfg_body) { body->read_cfg(*cfg_body); }
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
void Creator::updateBall(Cur& cur)
{
	if (active)
	{
		double r = sqrt((p_rec - cur.input.msp).lensqr());
		double min_r = 7.5;
		if (r > min_r)
		{
			mkp(body)(msh<Shape>(r));
			body->o = vector2(p_rec);
			body->innerColor = dColor(generateRadomInt(256), generateRadomInt(256), generateRadomInt(256));
			if (cfg_body) { body->read_cfg(*cfg_body); }
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
void Creator::updatePlate(Cur& cur)
{
	if (active)
	{
		double thk = thick;
		double min_len = 5;
		dVector2 v = p_rec - cur.input.msp;
		double len = sqrt(v.lensqr());
		if (len > min_len)
		{
			mkp(body)(msh<Shape>(getRectVerticesByCenter(len + thk, thk)));
			body->o = vector2(p_rec + cur.input.msp) / 2;
			body->radian = atan2(v.y, v.x);
			body->innerColor = dColor(generateRadomInt(256), generateRadomInt(256), generateRadomInt(256));
			if (cfg_body) { body->read_cfg(*cfg_body); }
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
void Creator::updateConnection(Cur& cur)
{
	if (active)
	{
		mkp(connection)();
		if (cfg_connection) { connection->read_cfg(*cfg_connection); }
		// Ҫ�� creator �ĸ����� body ����԰ɣ��ǲ������̫���ˡ�
		// ��Ҳûɶ�취�ǰɡ�
		if (!cur.input.msd[0])
		{
			if (body1)
			{
				connection->body0 = body0;
				connection->body1 = body1;
				connection->p0Relative = p0Relative;
				connection->p1Relative = p1Relative;

				connection->signUpToBodies();
				connection->updatePosition();
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
void Creator::updatePoint(Cur& cur)
{
	if (hovered && cur.input.isMouseClickedDown(0))
	{
		mkp(body)((vector2) cur.input.msp);
		body->innerColor = dColor(generateRadomInt(256), generateRadomInt(256), generateRadomInt(256));
		if (cfg_body) { body->read_cfg(*cfg_body); }
		body->Init();
		cur.bodies.push_back(body);
		body = NULL; 
		cur.isSceneChanged = true;
	}
}
void Creator::updateNail(Cur& cur)
{
	if (body0)
	{
		mkp(body)(body0->o + body0->transform * p0Relative);
		body->Init();
		cur.bodies.push_back(body);
		mkp(connection)();
		connection->type = CON_LINK;
		connection->body0 = body0;
		connection->body1 = &*body;
		connection->p0Relative = p0Relative;
		connection->signUpToBodies();
		connection->updatePosition();
		cur.connections.push_back(connection);

		body0 = NULL; connection = NULL;
		body = NULL; cur.isSceneChanged = true;
	}
}
void Creator::updateParticle(Cur& cur)
{
	if (hovered && cur.input.isMouseClickedDown(0))
	{
		mkp(body)(msh<Shape>(rad));
		body->o = (vector2) cur.input.msp;
		body->innerColor = dColor(generateRadomInt(256), generateRadomInt(256), generateRadomInt(256));
		if (cfg_body) { body->read_cfg(*cfg_body); }
		body->Init();
		cur.bodies.push_back(body);
		body = NULL; cur.isSceneChanged = true;
	}
}
void Creator::Update(Cur& cur)
{
	hovered = (cur.own.hovered == this);
	body = NULL; connection = NULL;

	switch (mode)
	{
		case CREATE_BOX: updateBox(cur); break;
		case CREATE_BALL: updateBall(cur); break;
		case CREATE_PLATE: updatePlate(cur); break;
		case CREATE_CONN: updateConnection(cur); break;
		case CREATE_POINT: updatePoint(cur); break;
		case CREATE_NAIL: updateNail(cur); break;
		case CREATE_PARTICLE: updateParticle(cur); break;
	}


	if (body) { body->Render(cur); }
	if (connection && body0)
	{
		connection->p0 = body0->o + body0->transform * p0Relative;
		connection->p1 = (vector2) cur.input.msp;
		connection->Render(cur);
	}
}
void Creator::Discard(Cur& cur)
{
	cur.own.remove(this); body0 = body1 = NULL;
	body = NULL; connection = NULL;
	hovered = active = false;
}
void Creator::PreUpdate(Cur& cur)
{
	if (cur.own.hoveredDepth <= dep() && isInside(cur.input.msp, cur.mBgr->viewPort()))
	{
		cur.own.hoveredDepth = dep();
		cur.own.hovered = this;
	}
}


