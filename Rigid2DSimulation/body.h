#pragma once
#include "tile.h"
#include "aabb.h"

struct proc;
typedef vector<proc> Program;

struct Var;
struct Cur;
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
	double dep_shape = 0, dep_point = 0;
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
	Body(Cur& cur, FILE* f);
	void save(FILE* f) const;

	vector2 generateRandomPointInside() const;
	bool inside(vector2 p) const;
	void refresh(Cur& cur);
	void read_cfg(Var const& cfg);
	void updatePositionAndAABB();
	void warp(dRect rc);
	void registerGrid(Cur& cur);
private:
	double getDepth() const;
	void updateAABB();

public:
	void Init(bool repos_o = false);
	void Render(Cur& cur) const;
	void Step(Cur& cur, double sdt);
	void Update(Cur& cur);
	void PreUpdate(Cur& cur);

private:
	void updateDragAngle(Cur& cur);
	void handleDragWhole(Cur& cur);
	void handleDragPoint(Cur& cur);
	void handleDragForce(Cur& cur, double sdt);
	void followPresetO(Cur& cur, double sdt);
	void followPresetAngle(Cur& cur, double sdt);
	void followPresetVelocityAngular(Cur& cur);

};

void Electrostatic(Body& b0, Body& b1, double sdt, double coulomb);
