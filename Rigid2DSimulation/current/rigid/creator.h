#pragma once
#include "vector2.h"

int constexpr CREATE_BOX = 0;
int constexpr CREATE_BALL = 1;
int constexpr CREATE_PLATE = 2;
int constexpr CREATE_PARTICLE = 3;
int constexpr CREATE_CONN = 4;
int constexpr CREATE_POINT = 5;
int constexpr CREATE_NAIL = 6;

struct Var;
struct SimulationApp;
struct Body;
struct Connection;
struct Creator
{
	int mode = 0;
	double depthBody = 0;
	double depthConnection = 0;
	double thick = 0;
	double rad = 0;
	dVector2 p_rec;
	Body* body0 = NULL;
	Body* body1 = NULL;
	ptr<Var> cfg_body, cfg_connection;
	vector2 p0Relative, p1Relative;

	ptr<Body> body;
	ptr<Connection> connection;
	bool active = false;
	bool hovered = false;

	Creator();
	Creator(Var const& v);
	double depth() const;
	void Update(SimulationApp& cur);
	void Discard(SimulationApp& cur);
	void PreUpdate(SimulationApp& cur);

private:
	void updateBox(SimulationApp& cur);
	void updateBall(SimulationApp& cur);
	void updatePlate(SimulationApp& cur);
	void updateConnection(SimulationApp& cur);
	/// <summary>
	/// 在一个空地方点击，开始创建一个新物体
	/// </summary>
	/// <param name="cur"></param>
	void updatePoint(SimulationApp& cur);
	/// <summary>
	/// 将body0固定钉在原地
	/// </summary>
	/// <param name="cur"></param>
	void updateNail(SimulationApp& cur);
	/// <summary>
	/// 在鼠标位置创建一个半径为 radius 的圆/粒子
	/// </summary>
	/// <param name="cur"></param>
	void updateParticle(SimulationApp& cur);
};


