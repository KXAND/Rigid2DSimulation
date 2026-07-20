#include "scene_builders.h"
#include "current/common/utils.h"
#include "simulation_app.h"
#include "background.h"
#include "body.h"
#include "shape.h"
#include "connection.h"

void CreateBody(SimulationApp& cur, Var const& shs_cfg, Var const& cfg)
{
	bool point = false;
	TryReadConfigNumber(cfg, L"point", point);
	ptr<Body> bo;
	vector<ptr<Shape>> shs;
	for (auto v : shs_cfg.vec)
	{
		if (v->vec.empty()) { continue; }
		ptr<Shape> sh;
		if (v->vec[0]->num)
		{
			if (v->vec.size() < 2) { continue; }
			vector2 o_rel;
			if (v->vec.size() >= 3) { o_rel = ToVector2(*v->vec[2]); }
			mkp(sh)(v->vec[1]->num, o_rel);
		}
		else
		{
			if (v->vec.size() < 2) { continue; }
			vector<vector2> vs;
			for (auto w : v->vec[1]->vec)
			{
				vs.push_back(ToVector2(*w));
			}
			mkp(sh)(vs);
		}
		shs.push_back(sh);
	}
	if (point) { mkp(bo)(vector2()); }
	else { mkp(bo)(shs); }
	bo->innerColor = dColor(GenerateRandomInt(256), GenerateRandomInt(256), GenerateRandomInt(256));
	bo->ReadCfg(cfg);
	bool repos_o = false;
	TryReadConfigNumber(cfg, L"repos_o", repos_o);
	bo->Init(repos_o);
	cur.bodies.push_back(bo);
	cur.isSceneChanged = true;
}
void CreateConnection(SimulationApp& cur, Var const& cfg)
{
	auto& bodies = cur.bodies;
	int idx0 = GenerateRandomInt(bodies.size());
	int idx1 = GenerateRandomInt(bodies.size());
	TryReadConfigNumber(cfg, L"idx0", idx0);
	TryReadConfigNumber(cfg, L"idx1", idx1);
	idx0 = clamp(idx0, 0, (int)bodies.size());
	idx1 = clamp(idx1, 0, (int)bodies.size());
	ptr<Connection> connection;
	mkp(connection)();
	connection->body0 = &*bodies[idx0];
	connection->body1 = &*bodies[idx1];
	connection->p0Relative = connection->body0->GenerateRandomPointInside();
	connection->p1Relative = connection->body1->GenerateRandomPointInside();
	bool absolute = false;
	TryReadConfigNumber(cfg, L"absolute", absolute);
	if (auto value = FindConfigValue(cfg, L"p0"))
	{
		connection->p0Relative = ToVector2(*value);
		if (absolute)
		{
			connection->p0Relative -= connection->body0->position;
			connection->p0Relative = connection->body0->transform.inverse() * connection->p0Relative;
		}
	}
	if (auto value = FindConfigValue(cfg, L"p1"))
	{
		connection->p1Relative = ToVector2(*value);
		if (absolute)
		{
			connection->p1Relative -= connection->body1->position;
			connection->p1Relative = connection->body1->transform.inverse() * connection->p1Relative;
		}
	}
	connection->SignUpToBodies();
	connection->UpdatePosition();
	connection->len = (connection->p0 - connection->p1).len();
	connection->ReadCfg(cfg);
	cur.connections.push_back(connection);
	cur.isSceneChanged = true;
}

void Boundary(SimulationApp& cur, Var const& thk, Var const& cfg)
{
	double bw = cur.background->width, bh = cur.background->height;
	double bx0 = cur.background->topLeft.x, bx1 = bx0 + bw;
	double by0 = cur.background->topLeft.y, by1 = by0 + bh;
	ptr<Body> body;
	dColor color = dColor(GenerateRandomInt(256), GenerateRandomInt(256), GenerateRandomInt(256));
	body = msh<Body>(msh<Shape>(GetRectVerticesByCenter(thk.num, bh)));
	body->position = vector2(bx0, (by0 + by1) / 2);
	body->fixed = true;
	body->innerColor = color;
	body->ReadCfg(cfg);
	body->Init();
	cur.bodies.push_back(body);
	body = msh<Body>(msh<Shape>(GetRectVerticesByCenter(thk.num, bh)));
	body->position = vector2(bx1, (by0 + by1) / 2);
	body->fixed = true;
	body->innerColor = color;
	body->ReadCfg(cfg);
	body->Init();
	cur.bodies.push_back(body);
	body = msh<Body>(msh<Shape>(GetRectVerticesByCenter(bw, thk.num)));
	body->position = vector2((bx0 + bx1) / 2, by0);
	body->fixed = true;
	body->innerColor = color;
	body->ReadCfg(cfg);
	body->Init();
	cur.bodies.push_back(body);
	body = msh<Body>(msh<Shape>(GetRectVerticesByCenter(bw, thk.num)));
	body->position = vector2((bx0 + bx1) / 2, by1);
	body->fixed = true;
	body->innerColor = color;
	body->ReadCfg(cfg);
	body->Init();
	cur.bodies.push_back(body);
	cur.isSceneChanged = true;
}
void Gear(SimulationApp& cur, Var const& r, Var const& n, Var const& h, Var const& cfg)
{
	vector<ptr<Shape>> shs;
	shs.push_back(msh<Shape>(r.num));
	rep(i, 0, n.num)
	{
		double phi0 = 2 * PI * i / (int)n.num;
		double phi1 = 2 * PI * (i + 1) / (int)n.num;
		double phi = (phi0 + phi1) / 2;
		vector2 v = vector2(cos(phi), sin(phi)) * (r.num + h.num);
		vector2 v0 = vector2(cos(phi0), sin(phi0)) * r.num;
		vector2 v1 = vector2(cos(phi1), sin(phi1)) * r.num;
		shs.push_back(msh<Shape>(vector<vector2>{ v0, v, v1 }));
	}
	auto wheel = msh<Body>(shs);
	wheel->innerColor = dColor(GenerateRandomInt(256), GenerateRandomInt(256), GenerateRandomInt(256));
	wheel->ReadCfg(cfg);
	wheel->Init();
	cur.bodies.push_back(wheel);
	auto root = msh<Body>(wheel->position);
	root->innerColor = wheel->innerColor;
	root->Init();
	cur.bodies.push_back(root);
	auto connection = msh<Connection>();
	connection->type = CON_LINK;
	connection->body0 = &*root;
	connection->body1 = &*wheel;
	connection->SignUpToBodies();
	connection->UpdatePosition();
	cur.connections.push_back(connection);
	cur.isSceneChanged = true;
}
void Strand(SimulationApp& cur, Var const& _p0, Var const& _p1,
	Var const& _n, Var const& ratio, Var const& bodyConfiguration, Var const& cfg_conn)
{
	vector2 p0 = ToVector2(_p0), p1 = ToVector2(_p1);
	int n = _n.num;
	ptr<Body> b0, b1;
	ptr<Connection> connection;
	double len = (p1 - p0).len() / n;
	double rad = 0.5 * ratio.num * len;
	double gap = max(0.0, len - 2 * rad);
	dColor col = dColor(GenerateRandomInt(256), GenerateRandomInt(256), GenerateRandomInt(256));
	mkp(b0)(msh<Shape>(rad));
	b0->innerColor = col;
	b0->position = p0;
	b0->ReadCfg(bodyConfiguration);
	b0->Init();
	cur.bodies.push_back(b0);
	rep(i, 1, n + 1)
	{
		mkp(b1)(msh<Shape>(rad));
		b1->innerColor = col;
		b1->position = (p0 * (n - i) + p1 * i) / n;
		b1->ReadCfg(bodyConfiguration);
		b1->Init();
		cur.bodies.push_back(b1);
		mkp(connection)();
		connection->body0 = &*b0;
		connection->body1 = &*b1;
		connection->len = gap;
		connection->p0Relative = +rad * (p1 - p0).normalize();
		connection->p1Relative = -rad * (p1 - p0).normalize();
		connection->ReadCfg(cfg_conn);
		connection->SignUpToBodies();
		connection->UpdatePosition();
		cur.connections.push_back(connection);
		b0 = b1;
	}
	cur.isSceneChanged = true;
}
void Necklace(SimulationApp& cur, Var const& _o, Var const& rad,
	Var const& _n, Var const& ratio, Var const& bodyConfiguration, Var const& cfg_conn)
{
	ptr<Body> beg, b0, b1;
	int n = _n.num;
	vector2 o = ToVector2(_o);
	ptr<Connection> connection;
	double len = 2 * PI * rad.num / n;
	double ball_rad = 0.5 * ratio.num * len;
	double gap = max(0.0, len - 2 * ball_rad);
	dColor col = dColor(GenerateRandomInt(256), GenerateRandomInt(256), GenerateRandomInt(256));
	mkp(b0)(msh<Shape>(ball_rad));
	b0->innerColor = col;
	b0->position = o + vector2(rad.num, 0);
	b0->ReadCfg(bodyConfiguration);
	b0->Init();
	cur.bodies.push_back(b0);
	beg = b0;
	rep(i, 1, n)
	{
		double phi = 2 * PI * i / n;
		mkp(b1)(msh<Shape>(ball_rad));
		b1->innerColor = col;
		b1->position = o + vector2(cos(phi), sin(phi)) * rad.num;
		b1->ReadCfg(bodyConfiguration);
		b1->Init();
		cur.bodies.push_back(b1);
		mkp(connection)();
		connection->body0 = &*b0;
		connection->body1 = &*b1;
		connection->len = gap;
		connection->p1Relative = ball_rad * vector2(+sin(phi), -cos(phi));
		phi = 2 * PI * (i - 1) / n;
		connection->p0Relative = ball_rad * vector2(-sin(phi), +cos(phi));
		connection->ReadCfg(cfg_conn);
		connection->SignUpToBodies();
		connection->UpdatePosition();
		cur.connections.push_back(connection);
		b0 = b1;
	}
	double phi = 2 * PI * n / n;
	b1 = beg;
	mkp(connection)();
	connection->body0 = &*b0;
	connection->body1 = &*b1;
	connection->len = gap;
	connection->p1Relative = ball_rad * vector2(+sin(phi), -cos(phi));
	phi = 2 * PI * (n - 1) / n;
	connection->p0Relative = ball_rad * vector2(-sin(phi), +cos(phi));
	connection->ReadCfg(cfg_conn);
	connection->SignUpToBodies();
	connection->UpdatePosition();
	cur.connections.push_back(connection);
	cur.isSceneChanged = true;
}