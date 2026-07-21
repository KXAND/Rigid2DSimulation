#pragma once
#include "Vector2.h"

int constexpr kCreateBox = 0;
int constexpr kCreateBall = 1;
int constexpr kCreatePlate = 2;
int constexpr kCreateParticle = 3;
int constexpr kCreateConnection = 4;
int constexpr kCreatePoint = 5;
int constexpr kCreateNail = 6;

struct Value;
struct SimulationApp;
struct Body;
struct Connection;
struct Creator
{
	int mode = 0;
	double bodyDepth = 0;
	double connectionDepth = 0;
	double thickness = 0;
	double radius = 0;
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
	Creator(Value const& config);
	double Depth() const;
	void Update(SimulationApp& simulationApp);
	void Discard(SimulationApp& simulationApp);
	void PreUpdate(SimulationApp& simulationApp);

private:
	void UpdateBox(SimulationApp& simulationApp);
	void UpdateBall(SimulationApp& simulationApp);
	void UpdatePlate(SimulationApp& simulationApp);
	void UpdateConnection(SimulationApp& simulationApp);
	/// <summary>
	/// 在一个空地方点击，开始创建一个新物体
	/// </summary>
	/// <param name="simulationApp"></param>
	void UpdatePoint(SimulationApp& simulationApp);
	/// <summary>
	/// 将body0固定钉在原地
	/// </summary>
	/// <param name="simulationApp"></param>
	void UpdateNail(SimulationApp& simulationApp);
	/// <summary>
	/// 在鼠标位置创建一个半径为 radius 的圆/粒子
	/// </summary>
	/// <param name="simulationApp"></param>
	void UpdateParticle(SimulationApp& simulationApp);
};


