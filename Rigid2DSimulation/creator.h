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
struct Cur;
struct Body;
struct Connection;
struct Creator
{
	int mode = 0;
	double dep_body = 0;
	double dep_con = 0;
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
	double dep() const;
	void Update(Cur& cur);
	void Discard(Cur& cur);
	void PreUpdate(Cur& cur);

private:
	void updateBox(Cur& cur);
	void updateBall(Cur& cur);
	void updatePlate(Cur& cur);
	void updateConnection(Cur& cur);
	/// <summary>
	/// 在一个空地方点击，开始创建一个新物体
	/// </summary>
	/// <param name="cur"></param>
	void updatePoint(Cur& cur);
	/// <summary>
	/// 将body0固定钉在原地
	/// </summary>
	/// <param name="cur"></param>
	void updateNail(Cur& cur);
	/// <summary>
	/// 在鼠标位置创建一个半径为 radius 的圆/粒子
	/// </summary>
	/// <param name="cur"></param>
	void updateParticle(Cur& cur);
};
