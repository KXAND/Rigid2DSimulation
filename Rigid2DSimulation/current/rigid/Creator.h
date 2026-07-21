#pragma once
#include "Vector2.h"

int constexpr CREATE_BOX = 0;
int constexpr CREATE_BALL = 1;
int constexpr CREATE_PLATE = 2;
int constexpr CREATE_PARTICLE = 3;
int constexpr CREATE_CONN = 4;
int constexpr CREATE_POINT = 5;
int constexpr CREATE_NAIL = 6;

struct Value;
struct SimulationApp;
struct Body;
struct Connection;
struct Creator
{
	int mode = 0;
	double DepthBody = 0;
	double DepthConnection = 0;
	double thick = 0;
	double rad = 0;
	DVector2 creationStartPoint;
	Body* body0 = NULL;
	Body* body1 = NULL;
	std::shared_ptr<Value> bodyConfiguration, connectionConfiguration;
	Vector2 p0Relative, p1Relative;

	std::shared_ptr<Body> body;
	std::shared_ptr<Connection> connection;
	bool active = false;
	bool hovered = false;

	Creator();
	Creator(Value const& v);
	double Depth() const;
	void Update(SimulationApp& cur);
	void Discard(SimulationApp& cur);
	void PreUpdate(SimulationApp& cur);

private:
	void UpdateBox(SimulationApp& cur);
	void UpdateBall(SimulationApp& cur);
	void UpdatePlate(SimulationApp& cur);
	void UpdateConnection(SimulationApp& cur);
	/// <summary>
	/// 在一个空地方点击，开始创建一个新物体
	/// </summary>
	/// <param name="cur"></param>
	void UpdatePoint(SimulationApp& cur);
	/// <summary>
	/// 将body0固定钉在原地
	/// </summary>
	/// <param name="cur"></param>
	void UpdateNail(SimulationApp& cur);
	/// <summary>
	/// 在鼠标位置创建一个半径为 radius 的圆/粒子
	/// </summary>
	/// <param name="cur"></param>
	void UpdateParticle(SimulationApp& cur);
};


