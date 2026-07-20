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
	bool isMarkedForDeletion = false;
	bool point = false;
	bool fixed = false;
	std::shared_ptr<Program> positionProgram, angleProgram, angularVelocityProgram;
	double inertia = 0;
	bool hasPositionPreset = false;
	bool hasAnglePreset = false;
	bool hasAngularVelocityPreset = false;
	vector<vector2> track;
	vector<std::shared_ptr<Shape>> shapes;
	vector<Connection*> connections;
	vector2 position, velocity;
	aabb box;
	matrix2 transform = matrix2::I();
	wstring scriptText, temporaryScriptText;
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
	vector2 relativeDragPoint;
	double depthShape = 0, depthPoint = 0;
	double pointRadius = 0;

	dColor innerColor;
	dColor borderColor;
	dColor hoveredColor;
	dColor draggedColor;
	dColor selectedColor;
	dColor selectedConColor;

	Body();
	Body(vector2 const& _o);
	Body(std::shared_ptr<Shape> sh);
	Body(vector<std::shared_ptr<Shape>> const& _shs);
	Body(SimulationApp& cur, FILE* file);
	void Save(FILE* file) const;

	vector2 GenerateRandomPointInside() const;
	bool IsInside(vector2 p) const;
	void RefreshFromScript(SimulationApp& cur);
	void ReadCfg(Var const& cfg);
	void UpdateShapesPositionAndAABB();
	void Warp(dRect rc);
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
	void UpdateDragAngle(SimulationApp& cur);
	void HandleDragWhole(SimulationApp& cur);
	void HandleDragPoint(SimulationApp& cur);
	void HandleDragForce(SimulationApp& cur, double sdt);
	void FollowPresetPosition(SimulationApp& cur, double sdt);
	void FollowPresetAngle(SimulationApp& cur, double sdt);
	void FollowPresetAngularVelocity(SimulationApp& cur);

};

void Electrostatic(Body& b0, Body& b1, double sdt, double coulomb);

