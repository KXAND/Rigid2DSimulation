#pragma once
#include "dvec.h"

struct Cur;
struct Body;
struct Connection;
/// <summary>
/// Group ��һϵ��ͨ�� connection ���ӵ�ͼ��
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
/// ������� body ����Ϊ group
/// </summary>
/// <param name="bodies"></param>
/// <returns></returns>
vector<ptr<Group>> FormGroups(vector<ptr<Body>> const& bodies);

