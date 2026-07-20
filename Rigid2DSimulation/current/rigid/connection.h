#pragma once
#include "tile.h"

int constexpr CON_ROPE = 0;
int constexpr CON_LINK = 1;
int constexpr CON_SPRING = 3;
int constexpr CON_CORD = 4;

struct Var;
struct SimulationApp;
struct Body;
/// <summary>
/// 定义一个两两连接关系，根据连接物理材质的不同，更新位置
/// </summary>
struct Connection {
	bool isMarkedForDeletion = false;
	int type = 0;
	wstring scriptText, temporaryScriptText;
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
	Connection(SimulationApp& cur, FILE* file);
	void Save(SimulationApp const& cur, FILE* file) const;

	void ReadCfg(Var const& cfg);
	void SignUpToBodies();
	void UpdatePosition();
	void RefreshFromScript(SimulationApp& cur);
	void SimulateRopeOrLink(bool useEqualPositionCorrection);
	void SimulateCordOrSpring(double sdt);
	void Simulate(double sdt, bool useEqualPositionCorrection);
	void Render(SimulationApp& cur) const;
};

Body* GetOtherBody(Body* b, Connection const& con);


