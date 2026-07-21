#include "Body.h"
#include "current/common/utils.h"
#include "SimulationApp.h"
#include "Background.h"
#include "Creator.h"
#include "Shape.h"
#include "DrawTriangle.h"
#include "DrawGeometry.h"
#include "DrawCurve.h"
#include "DrawLineSegment.h"
#include "Connection.h"

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
Body::Body(Vector2 const& _o) : Body()
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
void Body::Save(FILE* file) const
{
	int size = 0;
	fwrite(&(point), sizeof(point), 1, file);
	fwrite(&(fixed), sizeof(fixed), 1, file);
	size = shapes.size();
	fwrite(&(size), sizeof(size), 1, file);
	for (auto sh : shapes) { sh->Save(file); }
	fwrite(&(position), sizeof(position), 1, file);
	fwrite(&(velocity), sizeof(velocity), 1, file);
	size = (scriptText).size();
	fwrite(&(size), sizeof(size), 1, file);
	fwrite(&*(scriptText).begin(), sizeof(*(&*(scriptText).begin())), size, file);
	fwrite(&(radian), sizeof(radian), 1, file);
	fwrite(&(velocityAngular), sizeof(velocityAngular), 1, file);
	fwrite(&(density), sizeof(density), 1, file);
	fwrite(&(elasticity), sizeof(elasticity), 1, file);
	fwrite(&(frictionStatic), sizeof(frictionStatic), 1, file);
	fwrite(&(frictionDynamic), sizeof(frictionDynamic), 1, file);
	fwrite(&(dampCoeff), sizeof(dampCoeff), 1, file);
	fwrite(&(dampCoeffAngular), sizeof(dampCoeffAngular), 1, file);
	fwrite(&(chargeDensity), sizeof(chargeDensity), 1, file);
	fwrite(&(innerColor), sizeof(innerColor), 1, file);
}
Body::Body(SimulationApp& cur, FILE* file) : Body()
{
	int size = 0;
	fread(&(point), sizeof(point), 1, file);
	fread(&(fixed), sizeof(fixed), 1, file);
	fread(&(size), sizeof(size), 1, file);
	for (int i = (0), iEnd = int(size); i < iEnd; ++i)
	{
		shapes.push_back(std::make_shared<Shape>(file));
	}
	fread(&(position), sizeof(position), 1, file);
	fread(&(velocity), sizeof(velocity), 1, file);
	fread(&(size), sizeof(size), 1, file);
	(scriptText).resize(size);
	fread(&*(scriptText).begin(), sizeof(*(&*(scriptText).begin())), size, file);
	temporaryScriptText = scriptText;
	fread(&(radian), sizeof(radian), 1, file);
	fread(&(velocityAngular), sizeof(velocityAngular), 1, file);
	fread(&(density), sizeof(density), 1, file);
	fread(&(elasticity), sizeof(elasticity), 1, file);
	fread(&(frictionStatic), sizeof(frictionStatic), 1, file);
	fread(&(frictionDynamic), sizeof(frictionDynamic), 1, file);
	fread(&(dampCoeff), sizeof(dampCoeff), 1, file);
	fread(&(dampCoeffAngular), sizeof(dampCoeffAngular), 1, file);
	fread(&(chargeDensity), sizeof(chargeDensity), 1, file);
	fread(&(innerColor), sizeof(innerColor), 1, file);

	RefreshFromScript(cur);
}

double Body::GetDepth() const
{
	return point ? depthPoint : depthShape;
}
bool Body::IsInside(Vector2 p) const
{
	if (point) { return (p - position).GetLengthSquared() < pointRadius * pointRadius; }

	for (auto sh : shapes)
	{
		if (sh->IsInside(p)) return true;
	}
	return false;
}

void Body::RefreshFromScript(SimulationApp& cur)
{
	cur.currentScope[L"body"] = std::make_shared<Value>();
	Execute(cur.currentScope, Compile(scriptText));
	if (cur.currentScope.find(L"body") == cur.currentScope.end()) { return; }
	auto const& body_cfg = *cur.currentScope[L"body"];
	ReadCfg(body_cfg);

	// 这些单独放是因为它们无法只能通过 scriptText 获得，无法保存。
	TryReadConfigNumber(body_cfg, L"hasPositionPreset", hasPositionPreset);
	TryReadConfigNumber(body_cfg, L"hasAnglePreset", hasAnglePreset);
	TryReadConfigNumber(body_cfg, L"hasAngularVelocityPreset", hasAngularVelocityPreset);
	if (auto value = FindConfigValue(body_cfg, L"positionProgram")) { positionProgram = std::make_shared<Program>(value->bodyStatements); }
	if (auto value = FindConfigValue(body_cfg, L"angleProgram")) { angleProgram = std::make_shared<Program>(value->bodyStatements); }
	if (auto value = FindConfigValue(body_cfg, L"angularVelocityProgram")) { angularVelocityProgram = std::make_shared<Program>(value->bodyStatements); }
	Init();
}

void Body::ReadCfg(Value const& cfg)
{
	if (auto value = FindConfigValue(cfg, L"position")) { position = ToVector2(*value); }
	if (auto value = FindConfigValue(cfg, L"velocity")) { velocity = ToVector2(*value); }
	if (auto value = FindConfigValue(cfg, L"innerColor")) { innerColor = (Color)ToVector3(*value); }
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

Vector2 Body::GenerateRandomPointInside() const
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
	transform = Matrix2::Rotation(radian);
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
	box = Aabb();
	for (const auto& shape : shapes)
	{
		box = Aabb(box, shape->GetBoundingBox());
	}
}

// 这个方法已经过时了。
void Body::Warp(DRect rc)
{
	if (invMass == 0 || dragged) { return; }

	if (position.x < rc.GetLeft()) { position.x += rc.w; }
	else if (position.x > rc.GetRight()) { position.x -= rc.w; }
	if (position.y < rc.GetTop()) { position.y += rc.h; }
	else if (position.y > rc.GetBottom()) { position.y -= rc.h; }
}

void Body::RegisterGrid(SimulationApp& cur)
{
	if (point) { return; }

	auto const& top_left = cur.rectScene.topLeftPosition;
	int x0 = floor((box.x0 - top_left.x) / cur.gridSize);
	int y0 = floor((box.y0 - top_left.y) / cur.gridSize);
	int x1 = floor((box.x1 - top_left.x) / cur.gridSize);
	int y1 = floor((box.y1 - top_left.y) / cur.gridSize);
	x0 = Clamp(x0, 0, cur.gridNumX - 1);
	x1 = Clamp(x1, 0, cur.gridNumX - 1);
	y0 = Clamp(y0, 0, cur.gridNumY - 1);
	y1 = Clamp(y1, 0, cur.gridNumY - 1);
	if (x1 > x0 + 1 || y1 > y0 + 1)
	{
		// 是多个跨越多个grid的物体
		cur.multiGridBodies.push_back(this);
		return;
	}
	cur.grid[y0 * cur.gridNumX + x0].push_back(this);
}

void Body::Init(bool resetPosition)
{
	// 这里假设是之前没有乱改成员。
	if (point) { return; }

	area = 0; Vector2 center;
	inertia = 0;
	for (auto shapes : shapes)
	{
		shapes->area = 0; Vector2 sub_center;
		if (shapes->isCircle)
		{
			shapes->area = kPi * shapes->r * shapes->r;
			sub_center = shapes->area * shapes->relativePosition;
			inertia += shapes->area * (shapes->r * shapes->r) / 2;
		}
		else
		{
			for (int i = (0), iEnd = int(shapes->verticesRelative.size()); i < iEnd; ++i)
			{
				int j = (i + 1) % shapes->verticesRelative.size();
				auto p = shapes->verticesRelative[i], q = shapes->verticesRelative[j];
				double tri_area = Matrix2(p, q).Determinant() / 2;
				double tri_inertia = Dot(p, p) + Dot(p, q) + Dot(q, q);
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
	if (resetPosition) { position = center; }
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
	Color normalColor;
	switch (cur.display)
	{
		case DISPLAY_COLOR:
			normalColor = innerColor;
			break;
			if (point) { break; }
		case DISPLAY_ENERGY:
		{
			double energy = velocity.GetLengthSquared() + velocityAngular * velocityAngular * inertia / area;
			normalColor = (Color) Vector3(energy * cur.energyMultiplier, 0, 0);
			break;
		}
		case DISPLAY_CHARGES:
		{
			normalColor = chargeDensity > 0 ?
				(Color) Vector3(chargeDensity * cur.chargeMultiplier, 0, 0) :
				(Color) Vector3(0, 0, -chargeDensity * cur.chargeMultiplier);
			break;
		}
		default:
			break;
	}

	Color color =
		dragged ? draggedColor :
		(cur.bodySelecting == this) ? selectedColor :
		hovered ? hoveredColor :
		isSelectedConnection ? selectedConColor :
		normalColor;

	if (point)
	{
		drawEllipse(cur.screen, cur.depthBuffer, GetDepth(), position, pointRadius, pointRadius, cur.background->ViewPort(), color);
		DrawEllipseBorder(cur.screen, cur.depthBuffer, GetDepth(), position, pointRadius, pointRadius, cur.background->ViewPort(), borderColor, 20);
	}
	for (auto& shape : shapes)
	{
		if (shape->isCircle)
		{
			drawEllipse(cur.screen, cur.depthBuffer, GetDepth(),
				shape->position, shape->r, shape->r, cur.background->ViewPort(), color);
			DrawEllipseBorder(cur.screen, cur.depthBuffer, GetDepth(),
				shape->position, shape->r, shape->r, cur.background->ViewPort(), borderColor, 30);

			// 显示一条半径以便于我们判断圆的旋转角度
			auto peakPoint = Vector2(cos(shape->radian), sin(shape->radian)) * shape->r + shape->position;
			DrawLineSegment(cur.screen, cur.depthBuffer, shape->position, peakPoint, GetDepth(), cur.background->ViewPort(), borderColor);
			continue;
		}

		for (int i = (0), iEnd = int(shape->vertices.size()); i < iEnd; ++i)
		{
			int j = (i + 1) % shape->vertices.size();
			drawTriangle(cur.screen, cur.depthBuffer, cur.background->ViewPort(), shape->position, shape->vertices[i], shape->vertices[j], GetDepth(), color);
		}
		for (int i = (0), iEnd = int(shape->vertices.size()); i < iEnd; ++i)
		{
			int j = (i + 1) % shape->vertices.size();
			DrawLineSegment(cur.screen, cur.depthBuffer, shape->vertices[i], shape->vertices[j], GetDepth(), cur.background->ViewPort(), borderColor);
		}
	}

	if (dragged)
	{
		Vector2 p1 = Vector2(cur.input.mousePosition);
		Vector2 p0 = position + transform * relativeDragPoint;
		DrawLineSegment(cur.screen, cur.depthBuffer, p0, p1, GetDepth(), cur.background->ViewPort(), Color(0, 255, 255));
	}

	if (!track.empty())
	{
		Vector2 p0 = track.front();
		for (auto p1 : track)
		{
			DrawLineSegment(cur.screen, cur.depthBuffer, p0, p1, 10, cur.background->ViewPort(), Color(255, 0, 255));
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
		if (cur.input.mouseButtonDown[2]) { HandleDragWhole(cur); }
		else if (!cur.inputOwners.keyboardOwner && cur.input.keyDown[L'F']) { HandleDragForce(cur, stepDt); }
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
	scope[L"time"] = std::make_shared<Value>(cur.simulationTime);
	auto positionOld = ToVector2(Execute(ret, cur.currentScope, scope, *positionProgram)->elements);
	scope.clear(); scope[L"time"] = std::make_shared<Value>(cur.simulationTime + stepDt); ret = false;
	auto positionNew = ToVector2(Execute(ret, cur.currentScope, scope, *positionProgram)->elements);
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
	scope[L"time"] = std::make_shared<Value>(cur.simulationTime);
	auto radianOld = Execute(ret, cur.currentScope, scope, *angleProgram)->number;
	scope.clear();
	scope[L"time"] = std::make_shared<Value>(cur.simulationTime + stepDt);
	ret = false;
	auto radianNew = Execute(ret, cur.currentScope, scope, *angleProgram)->number;
	auto velocityAngularNew = (radianNew - radianOld) / stepDt;

	radian = radianNew;
	velocityAngular = velocityAngularNew;
}

void Body::FollowPresetAngularVelocity(SimulationApp& cur)
{
	if (!angularVelocityProgram) { return; }

	Scope sc;
	bool ret = false;
	sc[L"time"] = std::make_shared<Value>(cur.simulationTime);
	auto velocityAngularNew = Execute(ret, cur.currentScope, sc, *angularVelocityProgram)->number;

	velocityAngular = velocityAngularNew;
}

void Body::UpdateDragAngle(SimulationApp& cur)
{
	velocityAngularDrag = 0;
	if (!cur.inputOwners.keyboardOwner)
	{
		if (cur.input.keyDown[L'Q']) { velocityAngularDrag = -cur.velocityAngularDrag; }
		if (cur.input.keyDown[L'E']) { velocityAngularDrag = +cur.velocityAngularDrag; }
	}
	radianDrag = radian + velocityAngularDrag * cur.realDeltaTime;
}

void Body::HandleDragPoint(SimulationApp& cur)
{
	// 下列命名指定o,p,m三点为body中心、被拖拽点及鼠标现在点
	Vector2 mousePos = Vector2(cur.input.mousePosition);
	Vector2 pPos = position + transform * relativeDragPoint;
	Vector2 mouseVelocity = Vector2(cur.input.mousePosition - cur.input.previousMousePosition) / cur.realDeltaTime;
	Vector2 pmDir = (mousePos - pPos).GetNormalized();

	Vector2 opDir = pPos - position;
	opDir = Vector2(-opDir.y, opDir.x);
	Vector2 pVelocity = velocity + velocityAngular * opDir;
	double rd0 = Dot(opDir, pmDir);
	double inv_i0 = invMass + rd0 * rd0 * invInertia;

	Vector2 impulse = Dot(mouseVelocity - pVelocity, pmDir) * pmDir / inv_i0;

	velocity += impulse * invMass;
	velocityAngular += Dot(impulse, opDir) * invInertia;
	position += mousePos - pPos;
}

/// <summary>
/// 所有点同步被拖拽位移
/// </summary>
/// <param name="cur"></param>
void Body::HandleDragWhole(SimulationApp& cur)
{
	Vector2 mousePos = Vector2(cur.input.mousePosition);
	Vector2 dragPointPos = position + transform * relativeDragPoint;
	Vector2 newVelocity = Vector2(cur.input.mousePosition - cur.input.previousMousePosition) / cur.realDeltaTime;

	velocity = newVelocity;
	radian = radianDrag;
	velocityAngular = velocityAngularDrag;
	position += mousePos - dragPointPos; // 中心平行位移
}

void Body::HandleDragForce(SimulationApp& cur, double deltaTime)
{
	// 运动路径方向
	Vector2 p1 = Vector2(cur.input.mousePosition);
	Vector2 p0 = position + transform * relativeDragPoint;
	Vector2 d = (p1 - p0).GetNormalized();

	// 力
	double k = 1e6;
	Vector2 impulse = k * (p1 - p0).GetLength() * deltaTime * d;

	// 刚体半径
	Vector2 r0 = p0 - position;
	r0 = Vector2(-r0.y, r0.x);

	velocity += impulse * invMass;
	velocityAngular += Dot(impulse, r0) * invInertia;
}
void Body::Update(SimulationApp& cur)
{
	hovered = (cur.inputOwners.hovered == this);

	if (cur.mode == MODE_DEL && hovered && cur.input.IsMouseClickedDown(0)) { isMarkedForDeletion = true; }
	if (cur.mode == MODE_SEL && hovered && cur.input.IsMouseClickedDown(0)) { cur.bodySelecting = this; }
	if (dragged)
	{
		// 检查拖拽模式，应用函数
		UpdateDragAngle(cur);
		if (cur.input.mouseButtonDown[2])
		{
			HandleDragWhole(cur);
		}
		else if (!cur.inputOwners.keyboardOwner && cur.input.keyDown[L'F']) { }
		else if (invMass == 0)
		{
			HandleDragWhole(cur);
		}
		else
		{
			HandleDragPoint(cur);
		}
		dragged = cur.input.mouseButtonDown[0] && (cur.mode == MODE_DRAG);
	}
	else
	{
		// 检查 draged是否判断正确，更新被拖拽角度
		dragged = hovered && cur.input.IsMouseClickedDown(0) && (cur.mode == MODE_DRAG);
		if (dragged)
		{
			// p移到m，那么mo = 变换*原距离 = 变换*原相对位置=po，故有。
			relativeDragPoint = transform.Inverse() * ((Vector2) cur.input.mousePosition - position);
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
		if (cur.input.IsMouseClickedDown(0) && !cur.creator->active)
		{
			cur.creator->body0 = this;
			cur.creator->p0Relative = transform.Inverse() * ((Vector2) cur.input.mousePosition - position);
		}
		else if (!cur.input.mouseButtonDown[0] && cur.input.previousMouseButtonDown[0] && cur.creator->active)
		{
			cur.creator->body1 = this;
			cur.creator->p1Relative = transform.Inverse() * ((Vector2) cur.input.mousePosition - position);
		}
	}
}
void Body::PreUpdate(SimulationApp& cur)
{
	bool ok = cur.inputOwners.hoveredDepth <= GetDepth() &&
		IsInside((Vector2) cur.input.mousePosition) && ::IsInside(cur.input.mousePosition, cur.background->ViewPort());
	if (ok) { cur.inputOwners.hoveredDepth = GetDepth(); cur.inputOwners.hovered = this; }
}

void Electrostatic(Body& b0, Body& b1, double sdt, double coulombConstant)
{
	double dsqr = (b1.position - b0.position).GetLengthSquared();
	Vector2 d = (b1.position - b0.position).GetNormalized();
	Vector2 je = -b0.charge * b1.charge * d / dsqr * sdt * coulombConstant;
	b0.velocity += je * b0.invMass;
	b1.velocity -= je * b1.invMass;
}



