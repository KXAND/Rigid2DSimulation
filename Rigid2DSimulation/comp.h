#pragma once
#include "vector2.h"

struct Var;
struct Cur;
struct Body;
struct Connection;

shared_ptr<Var> Ball(Var const& r);
shared_ptr<Var> Box(Var const& w, Var const& h);
shared_ptr<Var> RegularPoly(Var const& n, Var const& r);
shared_ptr<Var> Parallelogram(Var const& w, Var const& h, Var const& s);
shared_ptr<Var> Trapezoid(Var const& t, Var const& b, Var const& h);
shared_ptr<Var> Cross(Var const& s, Var const& thk);
shared_ptr<Var> FramedBox(Var const& w, Var const& h, Var const& thk);

void CreateBody(Cur& cur, Var const& shs_cfg, Var const& cfg);

/// <summary>
/// 随机选取两个物体建立一条连接
/// </summary>
/// <param name="cur"></param>
/// <param name="cfg"></param>
void CreateConnetion(Cur& cur, Var const& cfg);
void Boundary(Cur& cur, Var const& thk, Var const& cfg);
void Gear(Cur& cur, Var const& r, Var const& n, Var const& h, Var const& cfg);
void Strand(Cur& cur, Var const& _p0, Var const& _p1,
	Var const& _n, Var const& ratio, Var const& cfg_body, Var const& cfg_conn);
void Necklace(Cur& cur, Var const& _o, Var const& rad,
	Var const& _n, Var const& ratio, Var const& cfg_body, Var const& cfg_conn);
