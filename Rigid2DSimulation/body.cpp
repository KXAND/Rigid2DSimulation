#include "body.h"
#include "cur.h"
#include "bgr.h"
#include "cur.h"
#include "creator.h"
#include "shape.h"
#include "draw_tri.h"
#include "draw_geo.h"
#include "draw_curve.h"
#include "draw_px_seg.h"
#include "connection.h"

#include "my_def.h"
#define tls (cur.rect_scene.topLeftPosition)
#define found(s) (dic.find(s) != dic.end())
#define getv(nm) if (found(L#nm)) { nm = dic.at(L#nm)->num; }

// ��Щ���캯����ĺܻ��ҡ�
Body::Body()
{
	density = 1;
	dampCoeff = 0.1; dampCoeffAngular = 0.1;
	elasticity = 0.7; frictionStatic = 0.5; frictionDynamic = 0.3;

	dep_shape = 0; dep_point = 1;
	r_point = 8;
	borderColor = { 255, 255, 255 };
	hoveredColor = { 0, 0, 255 };
	draggedColor = { 255, 0, 0 };
	selectedColor = { 255, 0, 255 };
	selectedConColor = { 255, 255, 0 };
}
Body::Body(vector2 const& _o) : Body()
{
	point = true;
	o = _o;
}
Body::Body(ptr<Shape> sh) : Body()
{
	shapes = { sh };
}
Body::Body(vector<ptr<Shape>> const& _shs) : Body()
{
	shapes = _shs;
}
void Body::save(FILE* f) const
{
	int sz = 0;
	fwt(point); fwt(fixed);
	sz = shapes.size();
	fwt(sz);
	for (auto sh : shapes) { sh->Save(f); }
	fwt(o); fwt(velocity); fwtv(cmd);
	fwt(radian); fwt(velocityAngular); fwt(density);
	fwt(elasticity); fwt(frictionStatic); fwt(frictionDynamic);
	fwt(dampCoeff); fwt(dampCoeffAngular);
	fwt(chargeDensity); fwt(innerColor);
}
Body::Body(Cur& cur, FILE* f) : Body()
{
	int sz = 0;
	frd(point); frd(fixed);
	frd(sz);
	rep(i, 0, sz)
	{
		shapes.push_back(msh<Shape>(f));
	}
	frd(o); frd(velocity); frdv(cmd); tmp_cmd = cmd;
	frd(radian); frd(velocityAngular); frd(density);
	frd(elasticity); frd(frictionStatic); frd(frictionDynamic);
	frd(dampCoeff); frd(dampCoeffAngular);
	frd(chargeDensity); frd(innerColor);

	refresh(cur);
}

double Body::getDepth() const
{
	return point ? dep_point : dep_shape;
}
bool Body::inside(vector2 p) const
{
	if (point) { return (p - o).lensqr() < r_point * r_point; }

	for (auto sh : shapes)
	{
		if (sh->isInside(p)) return true;
	}
	return false;
}

void Body::refresh(Cur& cur)
{
	gl[L"body"] = msh<Var>();
	Execute(gl, Compile(cmd));
	if (gl.find(L"body") == gl.end()) { return; }
	read_cfg(*gl[L"body"]);

	// ��Щ����������Ϊ�����޷�ֻ��ͨ�� cmd ��ã��޷����档
	auto dic = gl[L"body"]->dic;
	getv(preset_o); getv(preset_ang); getv(preset_v_ang);
	if (found(L"o_prog")) { mkp(o_prog)(dic.at(L"o_prog")->procs); }
	if (found(L"ang_prog")) { mkp(ang_prog)(dic.at(L"ang_prog")->procs); }
	if (found(L"v_ang_prog")) { mkp(v_ang_prog)(dic.at(L"v_ang_prog")->procs); }
	Init();
}

void Body::read_cfg(Var const& cfg)
{
	auto dic = cfg.dic;
	if (found(L"o")) { o = tv2(*dic[L"o"]); }
	if (found(L"v")) { velocity = tv2(*dic[L"v"]); }
	if (found(L"col")) { innerColor = (dColor) tv3(*dic[L"col"]); }
	getv(radian);
	getv(velocityAngular);
	getv(elasticity);
	getv(frictionStatic);
	getv(frictionDynamic);
	getv(dampCoeff);
	getv(dampCoeffAngular);
	getv(fixed);
	getv(density);
	getv(chargeDensity);
}

vector2 Body::generateRandomPointInside() const
{
	if (point) { return {}; }

	double randomArea = generateRadomFloat(area);
	for (const auto& shape : shapes)
	{
		randomArea -= shape->area;
		if (randomArea < 0)
		{
			return shape->generateRandomPointInside();
		}
	}
	return shapes.back()->generateRandomPointInside();
}

void Body::updatePositionAndAABB()
{
	transform = matrix2::rotate(radian);
	for (auto& shape : shapes)
	{
		shape->o = o + transform * shape->oRelative;
		shape->radian = radian;
		rep(i, 0, shape->vertices.size())
		{
			shape->vertices[i] = o + transform * shape->verticesRelative[i];
		}
	}
	updateAABB();
}

void Body::updateAABB()
{
	// @todo���������һ��expand����
	box = aabb();
	for (const auto& shape : shapes)
	{
		box = aabb(box, shape->getBoundingBox());
	}
}

void Body::warp(dRect rc)
{
	// ��������Ѿ���ʱ�ˡ�
	if (invMass == 0 || dragged) { return; }

	if (o.x < rc.left()) { o.x += rc.w; }
	else if (o.x > rc.right()) { o.x -= rc.w; }
	if (o.y < rc.top()) { o.y += rc.h; }
	else if (o.y > rc.bottom()) { o.y -= rc.h; }
}
void Body::register_grid(Cur& cur)
{
	if (point) { return; }

	int x0 = floor((box.x0 - tls.x) / cur.s_grid);
	int y0 = floor((box.y0 - tls.y) / cur.s_grid);
	int x1 = floor((box.x1 - tls.x) / cur.s_grid);
	int y1 = floor((box.y1 - tls.y) / cur.s_grid);
	x0 = clamp(x0, 0, cur.nx_grid - 1);
	x1 = clamp(x1, 0, cur.nx_grid - 1);
	y0 = clamp(y0, 0, cur.ny_grid - 1);
	y1 = clamp(y1, 0, cur.ny_grid - 1);
	if (x1 > x0 + 1 || y1 > y0 + 1)
	{
		cur.out_grid.push_back(this); return;
	}
	cur.grid[y0 * cur.nx_grid + x0].push_back(this);
}

void Body::Init(bool repos_o)
{
	// ���������֮ǰû���Ҹĳ�Ա��
	if (point) { return; }

	area = 0; vector2 center;
	inertia = 0;
	for (auto sh : shapes)
	{
		sh->area = 0; vector2 sub_center;
		if (sh->isCircle)
		{
			sh->area = PI * sh->r * sh->r;
			sub_center = sh->area * sh->oRelative;
			inertia += sh->area * (sh->r * sh->r) / 2;
		}
		else
		{
			rep(i, 0, sh->verticesRelative.size())
			{
				int j = (i + 1) % sh->verticesRelative.size();
				auto p = sh->verticesRelative[i], q = sh->verticesRelative[j];
				double tri_area = matrix2(p, q).det() / 2;
				double tri_inertia = dot(p, p) + dot(p, q) + dot(q, q);
				tri_inertia *= tri_area / 6;

				sub_center += tri_area * (p + q) / 3;
				sh->area += tri_area;
				inertia += tri_inertia;
			}
		}

		area += sh->area;
		center += sub_center;
		sh->oRelative = sub_center / sh->area;
	}

	center /= area;
	charge = chargeDensity * area;
	invMass = fixed ? 0 : 1 / (density * area);
	invInertia = fixed ? 0 : 1 / (density * inertia);
	if (repos_o) { o = center; }
	for (auto sh : shapes)
	{
		sh->oRelative -= center;
		for (auto& v : sh->verticesRelative) { v -= center; }
	}
	updatePositionAndAABB();
}
void Body::Render(Cur& cur) const
{
	bool selected_con = cur.con_sel &&
		(cur.con_sel->body0 == this || cur.con_sel->body1 == this);
	dColor normalColor;
	switch (cur.display)
	{
		case DISPLAY_COLOR:
			normalColor = innerColor;
			break;
			if (point) { break; }
		case DISPLAY_ENERGY:
		{
			double energy = velocity.lensqr() + velocityAngular * velocityAngular * inertia / area;
			normalColor = (dColor) vector3(energy * cur.energy_mul, 0, 0);
			break;
		}
		case DISPLAY_CHARGES:
		{
			normalColor = chargeDensity > 0 ?
				(dColor) vector3(chargeDensity * cur.chargeMultiplier, 0, 0) :
				(dColor) vector3(0, 0, -chargeDensity * cur.chargeMultiplier);
			break;
		}
		default:
			break;
	}

	dColor color =
		dragged ? draggedColor :
		(cur.body_sel == this) ? selectedColor :
		hovered ? hoveredColor :
		selected_con ? selectedConColor :
		normalColor;

	if (point)
	{
		drawEllipse(scr, dscr, getDepth(), o, r_point, r_point, bgr.viewPort(), color);
		drawEllipseBorder(scr, dscr, getDepth(), o, r_point, r_point, bgr.viewPort(), borderColor, 20);
	}
	for (auto& shape : shapes)
	{
		if (shape->isCircle)
		{
			drawEllipse(scr, dscr, getDepth(),
				shape->o, shape->r, shape->r, bgr.viewPort(), color);
			drawEllipseBorder(scr, dscr, getDepth(),
				shape->o, shape->r, shape->r, bgr.viewPort(), borderColor, 30);

			// ��ʾһ���뾶�Ա��������ж�Բ����ת�Ƕ�
			auto peakPoint = vector2(cos(shape->radian), sin(shape->radian)) * shape->r + shape->o;
			drawLineSegment(scr, dscr, shape->o, peakPoint, getDepth(), bgr.viewPort(), borderColor);
			continue;
		}

		rep(i, 0, shape->vertices.size())
		{
			int j = (i + 1) % shape->vertices.size();
			drawTriangle(scr, dscr, bgr.viewPort(), shape->o, shape->vertices[i], shape->vertices[j], getDepth(), color);
		}
		rep(i, 0, shape->vertices.size())
		{
			int j = (i + 1) % shape->vertices.size();
			drawLineSegment(scr, dscr, shape->vertices[i], shape->vertices[j], getDepth(), bgr.viewPort(), borderColor);
		}
	}

	if (dragged)
	{
		vector2 p1 = vector2(msp);
		vector2 p0 = o + transform * p_drag_rel;
		drawLineSegment(scr, dscr, p0, p1, getDepth(), bgr.viewPort(), dColor(0, 255, 255));
	}

	if (!track.empty())
	{
		vector2 p0 = track.front();
		for (auto p1 : track)
		{
			drawLineSegment(scr, dscr, p0, p1, 10, bgr.viewPort(), dColor(255, 0, 255));
			p0 = p1;
		}
	}
}

void Body::Step(Cur& cur, double stepDt)
{
	if (invMass)
	{
		velocity -= dampCoeff * velocity * stepDt;
		velocity += cur.gravity * stepDt;
		velocityAngular -= dampCoeffAngular * velocityAngular * stepDt;
		if (preset_v_ang) { followPresetVelocityAngular(cur); }
		o += velocity * stepDt;
		radian += velocityAngular * stepDt;
	}
	else
	{
		velocity = {};
		velocityAngular = 0;
		if (preset_o) { followPresetO(cur, stepDt); }
		if (preset_ang) { followPresetAngle(cur, stepDt); }
	}

	if (dragged)
	{
		if (msd[2]) { hdl_dragged_whole(cur); }
		else if (!kb && kbd[L'F']) { hdl_dragged_force(cur, stepDt); }
		else if (invMass == 0) { hdl_dragged_whole(cur); }
		else { hdl_dragged_point(cur); }
	}

	updatePositionAndAABB();
}

void Body::followPresetO(Cur& cur, double stepDt)
{
	if (!o_prog) { return; }
	Scope scope;
	bool ret = false;
	scope[L"t"] = msh<Var>(cur.t);
	auto positionOld = tv2(Execute(ret, gl, scope, *o_prog)->vec);
	scope.clear(); scope[L"t"] = msh<Var>(cur.t + stepDt); ret = false;
	auto positionNew = tv2(Execute(ret, gl, scope, *o_prog)->vec);
	auto velocityNew = (positionNew - positionOld) / stepDt;

	// ��ʵ�Ҿ���Ӧ��ѡ��ʱ���С��ֵ��
	o = positionNew;
	velocity = velocityNew;
}

void Body::followPresetAngle(Cur& cur, double stepDt)
{
	if (!ang_prog) { return; }

	Scope scope;
	bool ret = false;
	scope[L"t"] = msh<Var>(cur.t);
	auto radianOld = Execute(ret, gl, scope, *ang_prog)->num;
	scope.clear();
	scope[L"t"] = msh<Var>(cur.t + stepDt);
	ret = false;
	auto radianNew = Execute(ret, gl, scope, *ang_prog)->num;
	auto velocityAngularNew = (radianNew - radianOld) / stepDt;

	radian = radianNew;
	velocityAngular = velocityAngularNew;
}

void Body::followPresetVelocityAngular(Cur& cur)
{
	if (!v_ang_prog) { return; }

	Scope sc; bool ret = false;
	sc[L"t"] = msh<Var>(cur.t);
	auto velocityAngularNew = Execute(ret, gl, sc, *v_ang_prog)->num;

	velocityAngular = velocityAngularNew;
}

void Body::update_ang_drag(Cur& cur)
{
	v_ang_drag = 0;
	if (!kb)
	{
		if (kbd[L'Q']) { v_ang_drag = -cur.v_ang_drag; }
		if (kbd[L'E']) { v_ang_drag = +cur.v_ang_drag; }
	}
	ang_drag = radian + v_ang_drag * cur.real_dt;
}

void Body::hdl_dragged_point(Cur& cur)
{
	vector2 p1 = vector2(msp);
	vector2 p0 = o + transform * p_drag_rel;
	vector2 v1 = vector2(msp - msp_old) / cur.real_dt;
	vector2 d = (p1 - p0).normalize();

	vector2 r0 = p0 - o;
	r0 = vector2(-r0.y, r0.x);
	vector2 v0 = velocity + velocityAngular * r0;
	double rd0 = dot(r0, d);
	double inv_i0 = invMass + rd0 * rd0 * invInertia;
	vector2 j = dot(v1 - v0, d) * d / inv_i0;

	velocity += j * invMass;
	velocityAngular += dot(j, r0) * invInertia;
	o += p1 - p0;
}

void Body::hdl_dragged_whole(Cur& cur)
{
	vector2 p1 = vector2(msp);
	vector2 p0 = o + transform * p_drag_rel;
	vector2 v1 = vector2(msp - msp_old) / cur.real_dt;

	velocity = v1;
	radian = ang_drag;
	velocityAngular = v_ang_drag;
	o += p1 - p0;
}

void Body::hdl_dragged_force(Cur& cur, double sdt)
{
	vector2 p1 = vector2(msp);
	vector2 p0 = o + transform * p_drag_rel;
	vector2 d = (p1 - p0).normalize();

	vector2 r0 = p0 - o;
	r0 = vector2(-r0.y, r0.x);
	double hooke = 1e6;
	vector2 j = hooke * (p1 - p0).len() * sdt * d;

	velocity += j * invMass;
	velocityAngular += dot(j, r0) * invInertia;
}
void Body::Update(Cur& cur)
{
	hovered = (hvd == this);

	if (cur.mode == MODE_DEL && hovered && msc(0)) { del = true; }
	if (cur.mode == MODE_SEL && hovered && msc(0)) { cur.body_sel = this; }
	if (dragged)
	{
		update_ang_drag(cur);
		if (msd[2])
		{
			hdl_dragged_whole(cur);
		}
		else if (!kb && kbd[L'F']) { }
		else if (invMass == 0)
		{
			hdl_dragged_whole(cur);
		}
		else
		{
			hdl_dragged_point(cur);
		}
		dragged = msd[0] && (cur.mode == MODE_DRAG);
	}
	else
	{
		dragged = hovered && msc(0) && (cur.mode == MODE_DRAG);
		if (dragged)
		{
			p_drag_rel = transform.inverse() * ((vector2) msp - o);
			// ����������Ǻ���Ҫ�ġ�
			update_ang_drag(cur);
		}
	}

	if (this == cur.body_sel && cur.show_track && !cur.paused)
	{
		track.push_back(o);
	}
	if (this != cur.body_sel || !cur.show_track)
	{
		track.clear();
	}


	bool crt_ok = hovered && cur.mode == MODE_CREATE &&
		(cur.creator->mode == CREATE_CONN || cur.creator->mode == CREATE_NAIL);
	if (crt_ok)
	{
		if (msc(0) && !cur.creator->active)
		{
			cur.creator->b0 = this;
			cur.creator->p0_rel = transform.inverse() * ((vector2) msp - o);
		}
		else if (!msd[0] && msd_old[0] && cur.creator->active)
		{
			cur.creator->b1 = this;
			cur.creator->p1_rel = transform.inverse() * ((vector2) msp - o);
		}
	}
}
void Body::PreUpdate(Cur& cur)
{
	bool ok = dhv <= getDepth() &&
		inside((vector2) msp) && isInside(msp, bgr.viewPort());
	if (ok) { dhv = getDepth(); hvd = this; }
}

void Electrostatic(Body& b0, Body& b1, double sdt, double coulomb)
{
	double dsqr = (b1.o - b0.o).lensqr();
	vector2 d = (b1.o - b0.o).normalize();
	vector2 je = -b0.charge * b1.charge * d / dsqr * sdt * coulomb;
	b0.velocity += je * b0.invMass;
	b1.velocity -= je * b1.invMass;
}
