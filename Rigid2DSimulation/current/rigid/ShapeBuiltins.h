#pragma once
#include "framework/basic/Utils.h"

struct Value;

shared_ptr<Value> Ball(Value const& r);
shared_ptr<Value> Box(Value const& w, Value const& h);
shared_ptr<Value> RegularPolygon(Value const& n, Value const& r);
shared_ptr<Value> Parallelogram(Value const& w, Value const& h, Value const& s);
shared_ptr<Value> Trapezoid(Value const& t, Value const& b, Value const& h);
shared_ptr<Value> Cross(Value const& s, Value const& thk);
shared_ptr<Value> FramedBox(Value const& w, Value const& h, Value const& thk);