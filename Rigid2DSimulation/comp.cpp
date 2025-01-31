#include "comp.h"
#include "cur.h"
#include "var.h"
#include "bgr.h"
#include "body.h"
#include "shape.h"
#include "connection.h"

#include "my_def.h"
#define bodies (cur.bodies)
#define found(s) (dic.find(s) != dic.end())
#define getv(nm) if (found(L#nm)) { nm = dic.at(L#nm)->num; }

shared_ptr<Var> Ball(Var const& r)
{
	vector<shared_ptr<Var>> vs;
	vector<shared_ptr<Var>> v;
	// 应该有个快捷创建 Var 的方法才好。
	v.push_back(make_shared<Var>(1));
	v.push_back(make_shared<Var>(r));
	vs.push_back(make_shared<Var>(v));
	return make_shared<Var>(vs);
}
shared_ptr<Var> Box(Var const& w, Var const& h)
{
	vector<ptr<Var>> vs;
	vector<ptr<Var>> v;
	// 应该有个快捷创建 Var 的方法才好。
	v.push_back(msh<Var>(0));
	vector<ptr<Var>> tmp;
	tmp.push_back(msh<Var>(
		vector<ptr<Var>>{ msh<Var>(+w.num / 2), msh<Var>(+h.num / 2) }));
	tmp.push_back(msh<Var>(
		vector<ptr<Var>>{ msh<Var>(-w.num / 2), msh<Var>(+h.num / 2) }));
	tmp.push_back(msh<Var>(
		vector<ptr<Var>>{ msh<Var>(-w.num / 2), msh<Var>(-h.num / 2) }));
	tmp.push_back(msh<Var>(
		vector<ptr<Var>>{ msh<Var>(+w.num / 2), msh<Var>(-h.num / 2) }));
	v.push_back(msh<Var>(tmp));
	vs.push_back(msh<Var>(v));
	return msh<Var>(vs);
}
ptr<Var> RegularPoly(Var const& n, Var const& r)
{
	vector<ptr<Var>> vs;
	vector<ptr<Var>> v;
	v.push_back(msh<Var>(0));
	vector<ptr<Var>> tmp;

	auto m = max(3, (int) n.num);
	rep(i, 0, m)
	{
		double phi = 2 * PI * i / m;
		tmp.push_back(msh<Var>(
			vector<ptr<Var>>{
			msh<Var>(r.num* cos(phi)),
				msh<Var>(r.num* sin(phi)) }));
	}
	v.push_back(msh<Var>(tmp));
	vs.push_back(msh<Var>(v));
	return msh<Var>(vs);
}
ptr<Var> Parallelogram(Var const& w, Var const& h, Var const& s)
{
	vector<ptr<Var>> vs;
	vector<ptr<Var>> v;
	v.push_back(msh<Var>(0));
	vector<ptr<Var>> tmp;
	tmp.push_back(msh<Var>(
		vector<ptr<Var>>{ msh<Var>(+w.num / 2 + s.num), msh<Var>(+h.num / 2) }));
	tmp.push_back(msh<Var>(
		vector<ptr<Var>>{ msh<Var>(-w.num / 2 + s.num), msh<Var>(+h.num / 2) }));
	tmp.push_back(msh<Var>(
		vector<ptr<Var>>{ msh<Var>(-w.num / 2 - s.num), msh<Var>(-h.num / 2) }));
	tmp.push_back(msh<Var>(
		vector<ptr<Var>>{ msh<Var>(+w.num / 2 - s.num), msh<Var>(-h.num / 2) }));
	v.push_back(msh<Var>(tmp));
	vs.push_back(msh<Var>(v));
	return msh<Var>(vs);
}
ptr<Var> Trapezoid(Var const& t, Var const& b, Var const& h)
{
	vector<ptr<Var>> vs;
	vector<ptr<Var>> v;
	v.push_back(msh<Var>(0));
	vector<ptr<Var>> tmp;
	tmp.push_back(msh<Var>(
		vector<ptr<Var>>{ msh<Var>(+t.num / 2), msh<Var>(+h.num / 2) }));
	tmp.push_back(msh<Var>(
		vector<ptr<Var>>{ msh<Var>(-t.num / 2), msh<Var>(+h.num / 2) }));
	tmp.push_back(msh<Var>(
		vector<ptr<Var>>{ msh<Var>(-b.num / 2), msh<Var>(-h.num / 2) }));
	tmp.push_back(msh<Var>(
		vector<ptr<Var>>{ msh<Var>(+b.num / 2), msh<Var>(-h.num / 2) }));
	v.push_back(msh<Var>(tmp));
	vs.push_back(msh<Var>(v));
	return msh<Var>(vs);
}
ptr<Var> Cross(Var const& s, Var const& thk)
{
	vector<ptr<Var>> vs;
	vector<ptr<Var>> v;
	v.push_back(msh<Var>(0));
	vector<ptr<Var>> tmp;

	tmp.push_back(msh<Var>(
		vector<ptr<Var>>{ msh<Var>(+s.num / 2), msh<Var>(+thk.num / 2) }));
	tmp.push_back(msh<Var>(
		vector<ptr<Var>>{ msh<Var>(-s.num / 2), msh<Var>(+thk.num / 2) }));
	tmp.push_back(msh<Var>(
		vector<ptr<Var>>{ msh<Var>(-s.num / 2), msh<Var>(-thk.num / 2) }));
	tmp.push_back(msh<Var>(
		vector<ptr<Var>>{ msh<Var>(+s.num / 2), msh<Var>(-thk.num / 2) }));
	v.push_back(msh<Var>(tmp));
	vs.push_back(msh<Var>(v));

	v.clear();
	v.push_back(msh<Var>(0));
	tmp.clear();
	tmp.push_back(msh<Var>(
		vector<ptr<Var>>{ msh<Var>(+thk.num / 2), msh<Var>(+s.num / 2) }));
	tmp.push_back(msh<Var>(
		vector<ptr<Var>>{ msh<Var>(-thk.num / 2), msh<Var>(+s.num / 2) }));
	tmp.push_back(msh<Var>(
		vector<ptr<Var>>{ msh<Var>(-thk.num / 2), msh<Var>(-s.num / 2) }));
	tmp.push_back(msh<Var>(
		vector<ptr<Var>>{ msh<Var>(+thk.num / 2), msh<Var>(-s.num / 2) }));
	v.push_back(msh<Var>(tmp));
	vs.push_back(msh<Var>(v));

	return msh<Var>(vs);
}
ptr<Var> FramedBox(Var const& w, Var const& h, Var const& thk)
{
	double hw = 60, hh = 5;
	vector2 a0(+w.num / 2 + thk.num / 2, +h.num / 2 + thk.num / 2);
	vector2 a1(-w.num / 2 - thk.num / 2, +h.num / 2 + thk.num / 2);
	vector2 a2(-w.num / 2 - thk.num / 2, +h.num / 2 - thk.num / 2);
	vector2 a3(+w.num / 2 + thk.num / 2, +h.num / 2 - thk.num / 2);
	vector2 b0(+w.num / 2 + thk.num / 2, -h.num / 2 + thk.num / 2);
	vector2 b1(-w.num / 2 - thk.num / 2, -h.num / 2 + thk.num / 2);
	vector2 b2(-w.num / 2 - thk.num / 2, -h.num / 2 - thk.num / 2);
	vector2 b3(+w.num / 2 + thk.num / 2, -h.num / 2 - thk.num / 2);
	vector2 c0(+w.num / 2 + thk.num / 2, +h.num / 2 + thk.num / 2);
	vector2 c1(+w.num / 2 + thk.num / 2, -h.num / 2 - thk.num / 2);
	vector2 c2(+w.num / 2 - thk.num / 2, -h.num / 2 - thk.num / 2);
	vector2 c3(+w.num / 2 - thk.num / 2, +h.num / 2 + thk.num / 2);
	vector2 d0(-w.num / 2 + thk.num / 2, +h.num / 2 + thk.num / 2);
	vector2 d1(-w.num / 2 + thk.num / 2, -h.num / 2 - thk.num / 2);
	vector2 d2(-w.num / 2 - thk.num / 2, -h.num / 2 - thk.num / 2);
	vector2 d3(-w.num / 2 - thk.num / 2, +h.num / 2 + thk.num / 2);

	vector<ptr<Shape>> shapes;
	shapes.push_back(msh<Shape>(vector<vector2>{ a0, a1, a2, a3 }));
	shapes.push_back(msh<Shape>(vector<vector2>{ b0, b1, b2, b3 }));
	shapes.push_back(msh<Shape>(vector<vector2>{ c3, c2, c1, c0 }));
	shapes.push_back(msh<Shape>(vector<vector2>{ d3, d2, d1, d0 }));

	vector<ptr<Var>> vs;
	vector<ptr<Var>> v;
	v.push_back(msh<Var>(0));
	vector<ptr<Var>> tmp;

	tmp.push_back(msh<Var>(
		vector<ptr<Var>>{ msh<Var>(a0.x), msh<Var>(a0.y) }));
	tmp.push_back(msh<Var>(
		vector<ptr<Var>>{ msh<Var>(a1.x), msh<Var>(a1.y) }));
	tmp.push_back(msh<Var>(
		vector<ptr<Var>>{ msh<Var>(a2.x), msh<Var>(a2.y) }));
	tmp.push_back(msh<Var>(
		vector<ptr<Var>>{ msh<Var>(a3.x), msh<Var>(a3.y) }));
	v.push_back(msh<Var>(tmp));
	vs.push_back(msh<Var>(v));

	v.clear();
	v.push_back(msh<Var>(0));
	tmp.clear();
	tmp.push_back(msh<Var>(
		vector<ptr<Var>>{ msh<Var>(b0.x), msh<Var>(b0.y) }));
	tmp.push_back(msh<Var>(
		vector<ptr<Var>>{ msh<Var>(b1.x), msh<Var>(b1.y) }));
	tmp.push_back(msh<Var>(
		vector<ptr<Var>>{ msh<Var>(b2.x), msh<Var>(b2.y) }));
	tmp.push_back(msh<Var>(
		vector<ptr<Var>>{ msh<Var>(b3.x), msh<Var>(b3.y) }));
	v.push_back(msh<Var>(tmp));
	vs.push_back(msh<Var>(v));

	v.clear();
	v.push_back(msh<Var>(0));
	tmp.clear();
	tmp.push_back(msh<Var>(
		vector<ptr<Var>>{ msh<Var>(c3.x), msh<Var>(c3.y) }));
	tmp.push_back(msh<Var>(
		vector<ptr<Var>>{ msh<Var>(c2.x), msh<Var>(c2.y) }));
	tmp.push_back(msh<Var>(
		vector<ptr<Var>>{ msh<Var>(c1.x), msh<Var>(c1.y) }));
	tmp.push_back(msh<Var>(
		vector<ptr<Var>>{ msh<Var>(c0.x), msh<Var>(c0.y) }));
	v.push_back(msh<Var>(tmp));
	vs.push_back(msh<Var>(v));

	v.clear();
	v.push_back(msh<Var>(0));
	tmp.clear();
	tmp.push_back(msh<Var>(
		vector<ptr<Var>>{ msh<Var>(d3.x), msh<Var>(d3.y) }));
	tmp.push_back(msh<Var>(
		vector<ptr<Var>>{ msh<Var>(d2.x), msh<Var>(d2.y) }));
	tmp.push_back(msh<Var>(
		vector<ptr<Var>>{ msh<Var>(d1.x), msh<Var>(d1.y) }));
	tmp.push_back(msh<Var>(
		vector<ptr<Var>>{ msh<Var>(d0.x), msh<Var>(d0.y) }));
	v.push_back(msh<Var>(tmp));
	vs.push_back(msh<Var>(v));

	return msh<Var>(vs);
}

void CreateBody(Cur& cur, Var const& shs_cfg, Var const& cfg)
{
	bool point = false;
	auto dic = cfg.dic;
	// point 变量在 Body 之后不能变动，所以要特殊考虑。
	getv(point);
	ptr<Body> bo;
	vector<ptr<Shape>> shs;
	// 把这些抽象成 vec 的一个方法我觉得会更好。
	for (auto v : shs_cfg.vec)
	{
		if (v->vec.empty()) { continue; }
		ptr<Shape> sh;
		if (v->vec[0]->num)
		{
			if (v->vec.size() < 2) { continue; }
			vector2 o_rel;
			if (v->vec.size() >= 3) { o_rel = tv2(*v->vec[2]); }
			mkp(sh)(v->vec[1]->num, o_rel);
		}
		else
		{
			if (v->vec.size() < 2) { continue; }
			vector<vector2> vs;
			for (auto w : v->vec[1]->vec)
			{
				vs.push_back(tv2(*w));
			}
			mkp(sh)(vs);
		}
		shs.push_back(sh);
	}
	if (point) { mkp(bo)(vector2()); }
	else { mkp(bo)(shs); }

	bo->innerColor = dColor(generateRadomInt(256), generateRadomInt(256), generateRadomInt(256));
	bo->read_cfg(cfg);
	bool repos_o = false;
	getv(repos_o);
	bo->Init(repos_o);
	bodies.push_back(bo);
	cur.isSceneChanged = true;
}
void CreateConnetion(Cur& cur, Var const& cfg)
{
	int idx0 = generateRadomInt(bodies.size());
	int idx1 = generateRadomInt(bodies.size());
	auto dic = cfg.dic;
	getv(idx0); getv(idx1);
	idx0 = clamp(idx0, 0, (int) bodies.size());
	idx1 = clamp(idx1, 0, (int) bodies.size());

	ptr<Connection> connection;
	mkp(connection)();
	connection->body0 = &*bodies[idx0];
	connection->body1 = &*bodies[idx1];
	connection->p0Relative = connection->body0->generateRandomPointInside();
	connection->p1Relative = connection->body1->generateRandomPointInside();
	bool absolute = false;
	getv(absolute);
	if (found(L"p0"))
	{
		connection->p0Relative = tv2(*dic[L"p0"]);
		if (absolute)
		{
			connection->p0Relative -= connection->body0->o;
			connection->p0Relative = connection->body0->transform.inverse() * connection->p0Relative;
		}
	}
	if (found(L"p1"))
	{
		connection->p1Relative = tv2(*dic[L"p1"]);
		if (absolute)
		{
			connection->p1Relative -= connection->body1->o;
			connection->p1Relative = connection->body1->transform.inverse() * connection->p1Relative;
		}
	}
	connection->signUpToBodies();
	connection->updatePosition();
	connection->len = (connection->p0 - connection->p1).len();
	connection->read_cfg(cfg);
	cur.connections.push_back(connection);
	cur.isSceneChanged = true;
}

void Boundary(Cur& cur, Var const& thk, Var const& cfg)
{
	double bw = bgr.width, bh = bgr.height;
	double bx0 = bgr.topLeft.x, bx1 = bx0 + bw;
	double by0 = bgr.topLeft.y, by1 = by0 + bh;
	ptr<Body> body;
	dColor color = dColor(generateRadomInt(256), generateRadomInt(256), generateRadomInt(256));

	body = msh<Body>(msh<Shape>(getRectVerticesByCenter(thk.num, bh)));
	body->o = vector2(bx0, (by0 + by1) / 2);
	body->fixed = true;
	body->innerColor = color;
	body->read_cfg(cfg);
	body->Init();
	bodies.push_back(body);

	body = msh<Body>(msh<Shape>(getRectVerticesByCenter(thk.num, bh)));
	body->o = vector2(bx1, (by0 + by1) / 2);
	body->fixed = true;
	body->innerColor = color;
	body->read_cfg(cfg);
	body->Init();
	bodies.push_back(body);

	body = msh<Body>(msh<Shape>(getRectVerticesByCenter(bw, thk.num)));
	body->o = vector2((bx0 + bx1) / 2, by0);
	body->fixed = true;
	body->innerColor = color;
	body->read_cfg(cfg);
	body->Init();
	bodies.push_back(body);

	body = msh<Body>(msh<Shape>(getRectVerticesByCenter(bw, thk.num)));
	body->o = vector2((bx0 + bx1) / 2, by1);
	body->fixed = true;
	body->innerColor = color;
	body->read_cfg(cfg);
	body->Init();
	bodies.push_back(body);
	cur.isSceneChanged = true;
}
void Gear(Cur& cur, Var const& r, Var const& n, Var const& h, Var const& cfg)
{
	vector<ptr<Shape>> shs;
	shs.push_back(msh<Shape>(r.num));
	rep(i, 0, n.num)
	{
		double phi0 = 2 * PI * i / (int) n.num;
		double phi1 = 2 * PI * (i + 1) / (int) n.num;
		double phi = (phi0 + phi1) / 2;
		vector2 v = vector2(cos(phi), sin(phi)) * (r.num + h.num);
		vector2 v0 = vector2(cos(phi0), sin(phi0)) * r.num;
		vector2 v1 = vector2(cos(phi1), sin(phi1)) * r.num;
		shs.push_back(msh<Shape>(vector<vector2>{ v0, v, v1 }));
	}
	auto wheel = msh<Body>(shs);
	wheel->innerColor = dColor(generateRadomInt(256), generateRadomInt(256), generateRadomInt(256));
	wheel->read_cfg(cfg);
	wheel->Init();
	bodies.push_back(wheel);
	auto root = msh<Body>(wheel->o);
	root->innerColor = wheel->innerColor;
	root->Init();
	bodies.push_back(root);

	auto connection = msh<Connection>();
	connection->type = CON_LINK;
	connection->body0 = &*root;
	connection->body1 = &*wheel;
	connection->signUpToBodies();
	connection->updatePosition();
	cur.connections.push_back(connection);
	cur.isSceneChanged = true;
}
void Strand(Cur& cur, Var const& _p0, Var const& _p1,
	Var const& _n, Var const& ratio, Var const& cfg_body, Var const& cfg_conn)
{

	vector2 p0 = tv2(_p0), p1 = tv2(_p1);
	int n = _n.num;
	ptr<Body> b0, b1;
	ptr<Connection> connection;
	double len = (p1 - p0).len() / n;
	double rad = 0.5 * ratio.num * len;
	double gap = max(0.0, len - 2 * rad);
	dColor col = dColor(generateRadomInt(256), generateRadomInt(256), generateRadomInt(256));

	mkp(b0)(msh<Shape>(rad));
	b0->innerColor = col;
	b0->o = p0;
	b0->read_cfg(cfg_body);
	b0->Init();
	bodies.push_back(b0);

	rep(i, 1, n + 1)
	{
		mkp(b1)(msh<Shape>(rad));
		b1->innerColor = col;
		b1->o = (p0 * (n - i) + p1 * i) / n;
		b1->read_cfg(cfg_body);
		b1->Init();
		bodies.push_back(b1);

		mkp(connection)();
		connection->body0 = &*b0;
		connection->body1 = &*b1;
		connection->len = gap;
		connection->p0Relative = +rad * (p1 - p0).normalize();
		connection->p1Relative = -rad * (p1 - p0).normalize();
		connection->read_cfg(cfg_conn);
		connection->signUpToBodies();
		connection->updatePosition();
		cur.connections.push_back(connection);

		b0 = b1;
	}
	cur.isSceneChanged = true;
}
void Necklace(Cur& cur, Var const& _o, Var const& rad,
	Var const& _n, Var const& ratio, Var const& cfg_body, Var const& cfg_conn)
{

	ptr<Body> beg, b0, b1;
	int n = _n.num;
	vector2 o = tv2(_o);
	ptr<Connection> connection;
	double len = 2 * PI * rad.num / n;
	double ball_rad = 0.5 * ratio.num * len;
	double gap = max(0.0, len - 2 * ball_rad);
	dColor col = dColor(generateRadomInt(256), generateRadomInt(256), generateRadomInt(256));

	mkp(b0)(msh<Shape>(ball_rad));
	b0->innerColor = col;
	b0->o = o + vector2(rad.num, 0);
	b0->read_cfg(cfg_body);
	b0->Init();
	bodies.push_back(b0);
	beg = b0;

	rep(i, 1, n)
	{
		double phi = 2 * PI * i / n;
		mkp(b1)(msh<Shape>(ball_rad));
		b1->innerColor = col;
		b1->o = o + vector2(cos(phi), sin(phi)) * rad.num;
		b1->read_cfg(cfg_body);
		b1->Init();
		bodies.push_back(b1);

		mkp(connection)();
		connection->body0 = &*b0;
		connection->body1 = &*b1;
		connection->len = gap;
		connection->p1Relative = ball_rad * vector2(+sin(phi), -cos(phi));
		phi = 2 * PI * (i - 1) / n;
		connection->p0Relative = ball_rad * vector2(-sin(phi), +cos(phi));
		connection->read_cfg(cfg_conn);
		connection->signUpToBodies();
		connection->updatePosition();
		cur.connections.push_back(connection);

		b0 = b1;
	}
	double phi = 2 * PI * (n) / n;
	b1 = beg;
	mkp(connection)();
	connection->body0 = &*b0;
	connection->body1 = &*b1;
	connection->len = gap;
	connection->p1Relative = ball_rad * vector2(+sin(phi), -cos(phi));
	phi = 2 * PI * (n - 1) / n;
	connection->p0Relative = ball_rad * vector2(-sin(phi), +cos(phi));
	connection->read_cfg(cfg_conn);
	connection->signUpToBodies();
	connection->updatePosition();
	cur.connections.push_back(connection);
	cur.isSceneChanged = true;
}
