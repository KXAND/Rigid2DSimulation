#include "body.h"
#include "current/common/utils.h"
#include "simulation_app.h"
#include "background.h"
#include "creator.h"
#include "shape.h"
#include "draw_tri.h"
#include "draw_geo.h"
#include "draw_curve.h"
#include "draw_px_seg.h"
#include "connection.h"

// 这些构造函数真的很混乱。
Body::Body()
{
	density = 1;
	dampCoeff = 0.1; dampCoeffAngular = 0.1;
	elasticity = 0.7; frictionStatic = 0.5; frictionDynamic = 0.3;

	depthShape = 0; depthPoint = 1;
	pointRadius = 8;
	borderColor = { 255, 255, 255 };
	hoveredColor = { 0, 0, 255 };
	draggedColor = { 255, 0, 0 };
	selectedColor = { 255, 0, 255 };
	selectedConColor = { 255, 255, 0 };
}
Body::Body(vector2 const& _o) : Body()
{
	point = true;
	position = _o;
}
Body::Body(std::shared_ptr<Shape> sh) : Body()
{
	shapes = { sh };
}
Body::Body(vector<std::shared_ptr<Shape>> const& _shs) : Body()
{
	shapes = _shs;
}
void Body::Save(FILE* f) const
{
	int sz = 0;
	fwrite(&(point), sizeof(point), 1, f);
	fwrite(&(fixed), sizeof(fixed), 1, f);
	sz = shapes.size();
	fwrite(&(sz), sizeof(sz), 1, f);
	for (auto sh : shapes) { sh->Save(f); }
	fwrite(&(position), sizeof(position), 1, f);
	fwrite(&(velocity), sizeof(velocity), 1, f);
	sz = (scriptText).size();
	fwrite(&(sz), sizeof(sz), 1, f);
	fwrite(&*(scriptText).begin(), sizeof(*(&*(scriptText).begin())), sz, f);
	fwrite(&(radian), sizeof(radian), 1, f);
	fwrite(&(velocityAngular), sizeof(velocityAngular), 1, f);
	fwrite(&(density), sizeof(density), 1, f);
	fwrite(&(elasticity), sizeof(elasticity), 1, f);
	fwrite(&(frictionStatic), sizeof(frictionStatic), 1, f);
	fwrite(&(frictionDynamic), sizeof(frictionDynamic), 1, f);
	fwrite(&(dampCoeff), sizeof(dampCoeff), 1, f);
	fwrite(&(dampCoeffAngular), sizeof(dampCoeffAngular), 1, f);
	fwrite(&(chargeDensity), sizeof(chargeDensity), 1, f);
	fwrite(&(innerColor), sizeof(innerColor), 1, f);
}
Body::Body(SimulationApp& cur, FILE* f) : Body()
{
	int sz = 0;
	fread(&(point), sizeof(point), 1, f);
	fread(&(fixed), sizeof(fixed), 1, f);
	fread(&(sz), sizeof(sz), 1, f);
	for (int i = (0), iEnd = int(sz); i < iEnd; ++i)
	{
		shapes.push_back(std::make_shared<Shape>(f));
	}
	fread(&(position), sizeof(position), 1, f);
	fread(&(velocity), sizeof(velocity), 1, f);
	fread(&(sz), sizeof(sz), 1, f);
	(scriptText).resize(sz);
	fread(&*(scriptText).begin(), sizeof(*(&*(scriptText).begin())), sz, f);
	temporaryScriptText = scriptText;
	fread(&(radian), sizeof(radian), 1, f);
	fread(&(velocityAngular), sizeof(velocityAngular), 1, f);
	fread(&(density), sizeof(density), 1, f);
	fread(&(elasticity), sizeof(elasticity), 1, f);
	fread(&(frictionStatic), sizeof(frictionStatic), 1, f);
	fread(&(frictionDynamic), sizeof(frictionDynamic), 1, f);
	fread(&(dampCoeff), sizeof(dampCoeff), 1, f);
	fread(&(dampCoeffAngular), sizeof(dampCoeffAngular), 1, f);
	fread(&(chargeDensity), sizeof(chargeDensity), 1, f);
	fread(&(innerColor), sizeof(innerColor), 1, f);

	RefreshFromScript(cur);
}

double Body::GetDepth() const
{
	return point ? depthPoint : depthShape;
}
bool Body::IsInside(vector2 p) const
{
	if (point) { return (p - position).lensqr() < pointRadius * pointRadius; }

	for (auto sh : shapes)
	{
		if (sh->IsInside(p)) return true;
	}
	return false;
}

void Body::RefreshFromScript(SimulationApp& cur)
{
	cur.currentScope[L"body"] = std::make_shared<Var>();
	Execute(cur.currentScope, Compile(scriptText));
	if (cur.currentScope.find(L"body") == cur.currentScope.end()) { return; }
	auto const& body_cfg = *cur.currentScope[L"body"];
	ReadCfg(body_cfg);

	// 这些单独放是因为它们无法只能通过 scriptText 获得，无法保存。
	TryReadConfigNumber(body_cfg, L"preset_o", hasPositionPreset);
	TryReadConfigNumber(body_cfg, L"preset_ang", hasAnglePreset);
	TryReadConfigNumber(body_cfg, L"preset_v_ang", hasAngularVelocityPreset);
	if (auto value = FindConfigValue(body_cfg, L"o_prog")) { positionProgram = std::make_shared<Program>(value->procs); }
	if (auto value = FindConfigValue(body_cfg, L"ang_prog")) { angleProgram = std::make_shared<Program>(value->procs); }
	if (auto value = FindConfigValue(body_cfg, L"v_ang_prog")) { angularVelocityProgram = std::make_shared<Program>(value->procs); }
	Init();
}

void Body::ReadCfg(Var const& cfg)
{
	if (auto value = FindConfigValue(cfg, L"o")) { position = ToVector2(*value); }
	if (auto value = FindConfigValue(cfg, L"v")) { velocity = ToVector2(*value); }
	if (auto value = FindConfigValue(cfg, L"col")) { innerColor = (dColor)ToVector3(*value); }
	TryReadConfigNumber(cfg, L"radian", radian);
	TryReadConfigNumber(cfg, L"velocityAngular", velocityAngular);
	TryReadConfigNumber(cfg, L"elasticity", elasticity);
	TryReadConfigNumber(cfg, L"frictionStatic", frictionStatic);
	TryReadConfigNumber(cfg, L"frictionDynamic", frictionDynamic);
	TryReadConfigNumber(cfg, L"dampCoeff", dampCoeff);
	TryReadConfigNumber(cfg, L"dampCoeffAngular", dampCoeffAngular);
	TryReadConfigNumber(cfg, L"fixed", fixed);
	TryReadConfigNumber(cfg, L"density", density);
	TryReadConfigNumber(cfg, L"chargeDensity", chargeDensity);
}

vector2 Body::GenerateRandomPointInside() const
{
	if (point) { return {}; }

	double randomArea = GenerateRandomFloat(area);
	for (const auto& shape : shapes)
	{
		randomArea -= shape->area;
		if (randomArea < 0)
		{
			return shape->GenerateRandomPointInside();
		}
	}
	return shapes.back()->GenerateRandomPointInside();
}

void Body::UpdateShapesPositionAndAABB()
{
	transform = matrix2::rotate(radian);
	for (auto& shape : shapes)
	{
		shape->position = position + transform * shape->relativePosition;
		shape->radian = radian;
		for (int i = (0), iEnd = int(shape->vertices.size()); i < iEnd; ++i)
		{
			shape->vertices[i] = position + transform * shape->verticesRelative[i];
		}
	}
	UpdateAABB();
}

void Body::UpdateAABB()
{
	// @todo：考虑设计一个expand函数
	box = aabb();
	for (const auto& shape : shapes)
	{
		box = aabb(box, shape->GetBoundingBox());
	}
}

// 这个方法已经过时了。
void Body::Warp(dRect rc)
{
	if (invMass == 0 || dragged) { return; }

	if (position.x < rc.left()) { position.x += rc.w; }
	else if (position.x > rc.right()) { position.x -= rc.w; }
	if (position.y < rc.top()) { position.y += rc.h; }
	else if (position.y > rc.bottom()) { position.y -= rc.h; }
}

void Body::RegisterGrid(SimulationApp& cur)
{
	if (point) { return; }

	auto const& top_left = cur.rectScene.topLeftPosition;
	int x0 = floor((box.x0 - top_left.x) / cur.gridSize);
	int y0 = floor((box.y0 - top_left.y) / cur.gridSize);
	int x1 = floor((box.x1 - top_left.x) / cur.gridSize);
	int y1 = floor((box.y1 - top_left.y) / cur.gridSize);
	x0 = clamp(x0, 0, cur.gridNumX - 1);
	x1 = clamp(x1, 0, cur.gridNumX - 1);
	y0 = clamp(y0, 0, cur.gridNumY - 1);
	y1 = clamp(y1, 0, cur.gridNumY - 1);
	if (x1 > x0 + 1 || y1 > y0 + 1)
	{
		// 是多个跨越多个grid的物体
		cur.multiGridBodies.push_back(this);
		return;
	}
	cur.grid[y0 * cur.gridNumX + x0].push_back(this);
}

void Body::Init(bool repos_o)
{
	// 这里假设是之前没有乱改成员。
	if (point) { return; }

	area = 0; vector2 center;
	inertia = 0;
	for (auto shapes : shapes)
	{
		shapes->area = 0; vector2 sub_center;
		if (shapes->isCircle)
		{
			shapes->area = PI * shapes->r * shapes->r;
			sub_center = shapes->area * shapes->relativePosition;
			inertia += shapes->area * (shapes->r * shapes->r) / 2;
		}
		else
		{
			for (int i = (0), iEnd = int(shapes->verticesRelative.size()); i < iEnd; ++i)
			{
				int j = (i + 1) % shapes->verticesRelative.size();
				auto p = shapes->verticesRelative[i], q = shapes->verticesRelative[j];
				double tri_area = matrix2(p, q).det() / 2;
				double tri_inertia = dot(p, p) + dot(p, q) + dot(q, q);
				tri_inertia *= tri_area / 6;

				sub_center += tri_area * (p + q) / 3;
				shapes->area += tri_area;
				inertia += tri_inertia;
			}
		}

		area += shapes->area;
		center += sub_center;
		shapes->relativePosition = sub_center / shapes->area;
	}

	center /= area;
	charge = chargeDensity * area;
	invMass = fixed ? 0 : 1 / (density * area);
	invInertia = fixed ? 0 : 1 / (density * inertia);
	if (repos_o) { position = center; }
	for (auto sh : shapes)
	{
		sh->relativePosition -= center;
		for (auto& v : sh->verticesRelative) { v -= center; }
	}
	UpdateShapesPositionAndAABB();
}
void Body::Render(SimulationApp& cur) const
{
	bool isSelectedConnection = cur.connectionSelecting &&
		(cur.connectionSelecting->body0 == this || cur.connectionSelecting->body1 == this);
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
			normalColor = (dColor) vector3(energy * cur.energyMultiplier, 0, 0);
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
		(cur.bodySelecting == this) ? selectedColor :
		hovered ? hoveredColor :
		isSelectedConnection ? selectedConColor :
		normalColor;

	if (point)
	{
		drawEllipse(cur.scr, cur.dscr, GetDepth(), position, pointRadius, pointRadius, cur.background->ViewPort(), color);
		drawEllipseBorder(cur.scr, cur.dscr, GetDepth(), position, pointRadius, pointRadius, cur.background->ViewPort(), borderColor, 20);
	}
	for (auto& shape : shapes)
	{
		if (shape->isCircle)
		{
			drawEllipse(cur.scr, cur.dscr, GetDepth(),
				shape->position, shape->r, shape->r, cur.background->ViewPort(), color);
			drawEllipseBorder(cur.scr, cur.dscr, GetDepth(),
				shape->position, shape->r, shape->r, cur.background->ViewPort(), borderColor, 30);

			// 显示一条半径以便于我们判断圆的旋转角度
			auto peakPoint = vector2(cos(shape->radian), sin(shape->radian)) * shape->r + shape->position;
			drawLineSegment(cur.scr, cur.dscr, shape->position, peakPoint, GetDepth(), cur.background->ViewPort(), borderColor);
			continue;
		}

		for (int i = (0), iEnd = int(shape->vertices.size()); i < iEnd; ++i)
		{
			int j = (i + 1) % shape->vertices.size();
			drawTriangle(cur.scr, cur.dscr, cur.background->ViewPort(), shape->position, shape->vertices[i], shape->vertices[j], GetDepth(), color);
		}
		for (int i = (0), iEnd = int(shape->vertices.size()); i < iEnd; ++i)
		{
			int j = (i + 1) % shape->vertices.size();
			drawLineSegment(cur.scr, cur.dscr, shape->vertices[i], shape->vertices[j], GetDepth(), cur.background->ViewPort(), borderColor);
		}
	}

	if (dragged)
	{
		vector2 p1 = vector2(cur.input.msp);
		vector2 p0 = position + transform * relativeDragPoint;
		drawLineSegment(cur.scr, cur.dscr, p0, p1, GetDepth(), cur.background->ViewPort(), dColor(0, 255, 255));
	}

	if (!track.empty())
	{
		vector2 p0 = track.front();
		for (auto p1 : track)
		{
			drawLineSegment(cur.scr, cur.dscr, p0, p1, 10, cur.background->ViewPort(), dColor(255, 0, 255));
			p0 = p1;
		}
	}
}

void Body::Step(SimulationApp& cur, double stepDt)
{
	if (invMass)
	{
		velocity -= dampCoeff * velocity * stepDt;
		velocity += cur.gravity * stepDt;
		velocityAngular -= dampCoeffAngular * velocityAngular * stepDt;
		if (hasAngularVelocityPreset) { FollowPresetAngularVelocity(cur); }
		position += velocity * stepDt;
		radian += velocityAngular * stepDt;
	}
	else
	{
		velocity = {};
		velocityAngular = 0;
		if (hasPositionPreset) { FollowPresetPosition(cur, stepDt); }
		if (hasAnglePreset) { FollowPresetAngle(cur, stepDt); }
	}

	if (dragged)
	{
		if (cur.input.msd[2]) { HandleDragWhole(cur); }
		else if (!cur.own.keyboardOwner && cur.input.kbd[L'F']) { HandleDragForce(cur, stepDt); }
		else if (invMass == 0) { HandleDragWhole(cur); }
		else { HandleDragPoint(cur); }
	}

	UpdateShapesPositionAndAABB();
}

void Body::FollowPresetPosition(SimulationApp& cur, double stepDt)
{
	if (!positionProgram) { return; }
	Scope scope;
	bool ret = false;
	scope[L"t"] = std::make_shared<Var>(cur.t);
	auto positionOld = ToVector2(Execute(ret, cur.currentScope, scope, *positionProgram)->vec);
	scope.clear(); scope[L"t"] = std::make_shared<Var>(cur.t + stepDt); ret = false;
	auto positionNew = ToVector2(Execute(ret, cur.currentScope, scope, *positionProgram)->vec);
	auto velocityNew = (positionNew - positionOld) / stepDt;

	// 其实我觉得应当选用时间较小的值。
	position = positionNew;
	velocity = velocityNew;
}

void Body::FollowPresetAngle(SimulationApp& cur, double stepDt)
{
	if (!angleProgram) { return; }

	Scope scope;
	bool ret = false;
	scope[L"t"] = std::make_shared<Var>(cur.t);
	auto radianOld = Execute(ret, cur.currentScope, scope, *angleProgram)->num;
	scope.clear();
	scope[L"t"] = std::make_shared<Var>(cur.t + stepDt);
	ret = false;
	auto radianNew = Execute(ret, cur.currentScope, scope, *angleProgram)->num;
	auto velocityAngularNew = (radianNew - radianOld) / stepDt;

	radian = radianNew;
	velocityAngular = velocityAngularNew;
}

void Body::FollowPresetAngularVelocity(SimulationApp& cur)
{
	if (!angularVelocityProgram) { return; }

	Scope sc;
	bool ret = false;
	sc[L"t"] = std::make_shared<Var>(cur.t);
	auto velocityAngularNew = Execute(ret, cur.currentScope, sc, *angularVelocityProgram)->num;

	velocityAngular = velocityAngularNew;
}

void Body::UpdateDragAngle(SimulationApp& cur)
{
	velocityAngularDrag = 0;
	if (!cur.own.keyboardOwner)
	{
		if (cur.input.kbd[L'Q']) { velocityAngularDrag = -cur.velocityAngularDrag; }
		if (cur.input.kbd[L'E']) { velocityAngularDrag = +cur.velocityAngularDrag; }
	}
	radianDrag = radian + velocityAngularDrag * cur.realDeltaTime;
}

void Body::HandleDragPoint(SimulationApp& cur)
{
	// 下列命名指定o,p,m三点为body中心、被拖拽点及鼠标现在点
	vector2 mousePos = vector2(cur.input.msp);
	vector2 pPos = position + transform * relativeDragPoint;
	vector2 mouseVelocity = vector2(cur.input.msp - cur.input.msp_old) / cur.realDeltaTime;
	vector2 pmDir = (mousePos - pPos).normalize();

	vector2 opDir = pPos - position;
	opDir = vector2(-opDir.y, opDir.x);
	vector2 pVelocity = velocity + velocityAngular * opDir;
	double rd0 = dot(opDir, pmDir);
	double inv_i0 = invMass + rd0 * rd0 * invInertia;

	vector2 impulse = dot(mouseVelocity - pVelocity, pmDir) * pmDir / inv_i0;

	velocity += impulse * invMass;
	velocityAngular += dot(impulse, opDir) * invInertia;
	position += mousePos - pPos;
}

/// <summary>
/// 所有点同步被拖拽位移
/// </summary>
/// <param name="cur"></param>
void Body::HandleDragWhole(SimulationApp& cur)
{
	vector2 mousePos = vector2(cur.input.msp);
	vector2 dragPointPos = position + transform * relativeDragPoint;
	vector2 newVelocity = vector2(cur.input.msp - cur.input.msp_old) / cur.realDeltaTime;

	velocity = newVelocity;
	radian = radianDrag;
	velocityAngular = velocityAngularDrag;
	position += mousePos - dragPointPos; // 中心平行位移
}

void Body::HandleDragForce(SimulationApp& cur, double deltaTime)
{
	// 运动路径方向
	vector2 p1 = vector2(cur.input.msp);
	vector2 p0 = position + transform * relativeDragPoint;
	vector2 d = (p1 - p0).normalize();

	// 力
	double k = 1e6;
	vector2 impulse = k * (p1 - p0).len() * deltaTime * d;

	// 刚体半径
	vector2 r0 = p0 - position;
	r0 = vector2(-r0.y, r0.x);

	velocity += impulse * invMass;
	velocityAngular += dot(impulse, r0) * invInertia;
}
void Body::Update(SimulationApp& cur)
{
	hovered = (cur.own.hovered == this);

	if (cur.mode == MODE_DEL && hovered && cur.input.isMouseClickedDown(0)) { isMarkedForDeletion = true; }
	if (cur.mode == MODE_SEL && hovered && cur.input.isMouseClickedDown(0)) { cur.bodySelecting = this; }
	if (dragged)
	{
		// 检查拖拽模式，应用函数
		UpdateDragAngle(cur);
		if (cur.input.msd[2])
		{
			HandleDragWhole(cur);
		}
		else if (!cur.own.keyboardOwner && cur.input.kbd[L'F']) { }
		else if (invMass == 0)
		{
			HandleDragWhole(cur);
		}
		else
		{
			HandleDragPoint(cur);
		}
		dragged = cur.input.msd[0] && (cur.mode == MODE_DRAG);
	}
	else
	{
		// 检查 draged是否判断正确，更新被拖拽角度
		dragged = hovered && cur.input.isMouseClickedDown(0) && (cur.mode == MODE_DRAG);
		if (dragged)
		{
			// p移到m，那么mo = 变换*原距离 = 变换*原相对位置=po，故有。
			relativeDragPoint = transform.inverse() * ((vector2) cur.input.msp - position);
			UpdateDragAngle(cur);
		}
	}

	if (this == cur.bodySelecting && cur.isTrackShown && !cur.isPaused)
	{
		// 绘制运动轨迹
		track.push_back(position);
	}
	else if (this != cur.bodySelecting || !cur.isTrackShown)
	{
		//不符合条件时清空轨迹信息
		track.clear();
	}

	// 创造连接
	bool canCreate = hovered && cur.mode == MODE_CREATE &&
		(cur.creator->mode == CREATE_CONN || cur.creator->mode == CREATE_NAIL);
	if (canCreate)
	{
		if (cur.input.isMouseClickedDown(0) && !cur.creator->active)
		{
			cur.creator->body0 = this;
			cur.creator->p0Relative = transform.inverse() * ((vector2) cur.input.msp - position);
		}
		else if (!cur.input.msd[0] && cur.input.msd_old[0] && cur.creator->active)
		{
			cur.creator->body1 = this;
			cur.creator->p1Relative = transform.inverse() * ((vector2) cur.input.msp - position);
		}
	}
}
void Body::PreUpdate(SimulationApp& cur)
{
	bool ok = cur.own.hoveredDepth <= GetDepth() &&
		IsInside((vector2) cur.input.msp) && isInside(cur.input.msp, cur.background->ViewPort());
	if (ok) { cur.own.hoveredDepth = GetDepth(); cur.own.hovered = this; }
}

void Electrostatic(Body& b0, Body& b1, double sdt, double coulomb)
{
	double dsqr = (b1.position - b0.position).lensqr();
	vector2 d = (b1.position - b0.position).normalize();
	vector2 je = -b0.charge * b1.charge * d / dsqr * sdt * coulomb;
	b0.velocity += je * b0.invMass;
	b1.velocity -= je * b1.invMass;
}



