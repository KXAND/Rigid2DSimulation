#pragma once
#include "framework/basic/Utils.h"

struct Value;

shared_ptr<Value> Ball(Value const& radius);
shared_ptr<Value> Box(Value const& width, Value const& height);
shared_ptr<Value> RegularPolygon(Value const& sideCount, Value const& radius);
shared_ptr<Value> Parallelogram(Value const& width, Value const& height, Value const& shear);
shared_ptr<Value> Trapezoid(Value const& topWidth, Value const& bottomWidth, Value const& height);
shared_ptr<Value> Cross(Value const& size, Value const& thickness);
shared_ptr<Value> FramedBox(Value const& width, Value const& height, Value const& thickness);
