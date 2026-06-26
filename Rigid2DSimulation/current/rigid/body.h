#pragma once
#include "tile.h"
#include "aabb.h"

struct ScriptStatement;
typedef vector<ScriptStatement> Program;

struct Var;
struct SimulationApp;
struct Shape;
struct Connection;
/// <summary>
/// 定义单个物体的物理属性
/// </summary>
struct Body
{
	bool del = false;
	bool point = false;
	bool fixed = false;
	ptr<Program> o_prog, ang_prog, v_ang_prog;
	double inertia = 0;
	bool preset_o = false;
	bool preset_ang = false;
	bool preset_v_ang = false;
	vector<vector2> track;
	vector<ptr<Shape>> shapes;
	vector<Connection*> connections;
	vector2 o, velocity;
	aabb box;
	matrix2 transform = matrix2::I();
	wstring cmd, tmp_cmd;
	double radian = 0, velocityAngular = 0;
	double invMass = 0;// 质量倒数，为0表示此物体不参与运动计算
	double invInertia = 0;// 转动惯量倒数
	double area = 0, density = 0;
	double elasticity = 0;// 弹性系数
	double frictionStatic = 0, frictionDynamic = 0;
	double dampCoeff = 0, dampCoeffAngular = 0;
	double chargeDensity = 0, charge = 0;

	bool visited = false;
	bool hovered = false;
	bool dragged = false;
	double radianDrag = 0, velocityAngularDrag = 0;
	vector2 pDragRelative;
	double depthShape = 0, depthPoint = 0;
	double r_point = 0;

	dColor innerColor;
	dColor borderColor;
	dColor hoveredColor;
	dColor draggedColor;
	dColor selectedColor;
	dColor selectedConColor;

	Body();
	Body(vector2 const& _o);
	Body(ptr<Shape> sh);
	Body(vector<ptr<Shape>> const& _shs);
	Body(SimulationApp& cur, FILE* f);
	void save(FILE* f) const;

	vector2 GenerateRandomPointInside() const;
	bool inside(vector2 p) const;
	void refresh(SimulationApp& cur);
	void ReadCfg(Var const& cfg);
	void UpdateShapesPositionAndAABB();
	void warp(dRect rc);
	void RegisterGrid(SimulationApp& cur);
private:
	double GetDepth() const;
	void UpdateAABB();

public:
	void Init(bool repos_o = false);
	void Render(SimulationApp& cur) const;
	void Step(SimulationApp& cur, double sdt);
	void Update(SimulationApp& cur);
	void PreUpdate(SimulationApp& cur);

private:
	void updateDragAngle(SimulationApp& cur);
	void handleDragWhole(SimulationApp& cur);
	void handleDragPoint(SimulationApp& cur);
	void handleDragForce(SimulationApp& cur, double sdt);
	void followPresetO(SimulationApp& cur, double sdt);
	void followPresetAngle(SimulationApp& cur, double sdt);
	void followPresetVelocityAngular(SimulationApp& cur);

};

void Electrostatic(Body& b0, Body& b1, double sdt, double coulomb);

