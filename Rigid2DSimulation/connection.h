#pragma once
#include "tile.h"

int constexpr CON_ROPE = 0;
int constexpr CON_LINK = 1;
int constexpr CON_SPRING = 3;
int constexpr CON_CORD = 4;

struct Var;
struct Cur;
struct Body;
/// <summary>
/// 定义一个两两连接关系，根据连接物理材质的不同，更新位置
/// </summary>
struct Connection {
	bool del = false;
	int type = 0;
	wstring cmd, tmp_cmd;
	Body* body0 = NULL;
	Body* body1 = NULL;
	vector2 p0Relative, p1Relative;
	vector2 p0, p1;
	double len = 0;
	double hooke = 0;
	bool isTight = false;

	// @todo:应该根据变connection的类型决定这几个量的出现的
	dColor colorRopeTight, colorRope;
	dColor colorLink;
	dColor colorCordTight, colorCord;
	dColor colorSpringStretch, colorSpringCompress;

	Connection();
	Connection(Cur& cur, FILE* f);
	void save(Cur const& cur, FILE* f) const;

	void read_cfg(Var const& cfg);
	void signUpToBodies();
	void updatePosition();
	void refresh(Cur& cur);
	void simulateTypeRopeLink(bool equal_repos);
	void simulateTypeCordSpring(double sdt);
	void Simulate(double sdt, bool equal_repos);
	void Render(Cur& cur) const;
};

Body* getTheOtherBody(Body* b, Connection const& con);
