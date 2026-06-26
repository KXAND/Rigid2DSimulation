#pragma once
#include "framework/basic/utils.h"

struct Var;

shared_ptr<Var> Ball(Var const& r);
shared_ptr<Var> Box(Var const& w, Var const& h);
shared_ptr<Var> RegularPoly(Var const& n, Var const& r);
shared_ptr<Var> Parallelogram(Var const& w, Var const& h, Var const& s);
shared_ptr<Var> Trapezoid(Var const& t, Var const& b, Var const& h);
shared_ptr<Var> Cross(Var const& s, Var const& thk);
shared_ptr<Var> FramedBox(Var const& w, Var const& h, Var const& thk);