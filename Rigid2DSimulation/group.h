#pragma once
#include "dvec.h"

struct Cur;
struct Body;
struct Connection;
/// <summary>
/// Group 是一系列通过 connection 连接的图形
/// </summary>
struct Group {
	vector<Body*> bodies;
	Group(vector<Body*> const& bodies) : bodies(bodies) {}
	/// <summary>
	/// @todo
	/// </summary>
	/// <param name="rect"></param>
	void Warp(dRect rect);
};

void findBodiesInGroup(vector<Body*>& out, Body* b);
/// <summary>
/// 将输入的 body 划分为 group
/// </summary>
/// <param name="bodies"></param>
/// <returns></returns>
vector<ptr<Group>> FormGroups(vector<ptr<Body>> const& bodies);
