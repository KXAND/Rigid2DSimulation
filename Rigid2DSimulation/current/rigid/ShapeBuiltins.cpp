#include "ShapeBuiltins.h"
#include "Value.h"
#include "Shape.h"

shared_ptr<Value> Ball(Value const& radius)
{
	vector<shared_ptr<Value>> vs;
	vector<shared_ptr<Value>> v;
	v.push_back(make_shared<Value>(1));
	v.push_back(make_shared<Value>(radius));
	vs.push_back(make_shared<Value>(v));
	return make_shared<Value>(vs);
}
shared_ptr<Value> Box(Value const& width, Value const& height)
{
	vector<std::shared_ptr<Value>> vs;
	vector<std::shared_ptr<Value>> v;
	v.push_back(std::make_shared<Value>(0));
	vector<std::shared_ptr<Value>> tmp;
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(+width.number / 2), std::make_shared<Value>(+height.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(-width.number / 2), std::make_shared<Value>(+height.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(-width.number / 2), std::make_shared<Value>(-height.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(+width.number / 2), std::make_shared<Value>(-height.number / 2) }));
	v.push_back(std::make_shared<Value>(tmp));
	vs.push_back(std::make_shared<Value>(v));
	return std::make_shared<Value>(vs);
}
std::shared_ptr<Value> RegularPolygon(Value const& sideCount, Value const& radius)
{
	vector<std::shared_ptr<Value>> vs;
	vector<std::shared_ptr<Value>> v;
	v.push_back(std::make_shared<Value>(0));
	vector<std::shared_ptr<Value>> tmp;
	auto m = max(3, (int)sideCount.number);
	for (int i = (0), iEnd = int(m); i < iEnd; ++i)
	{
		double phi = 2 * kPi * i / m;
		tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(radius.number * cos(phi)), std::make_shared<Value>(radius.number * sin(phi)) }));
	}
	v.push_back(std::make_shared<Value>(tmp));
	vs.push_back(std::make_shared<Value>(v));
	return std::make_shared<Value>(vs);
}
std::shared_ptr<Value> Parallelogram(Value const& width, Value const& height, Value const& shear)
{
	vector<std::shared_ptr<Value>> vs;
	vector<std::shared_ptr<Value>> v;
	v.push_back(std::make_shared<Value>(0));
	vector<std::shared_ptr<Value>> tmp;
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(+width.number / 2 + shear.number), std::make_shared<Value>(+height.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(-width.number / 2 + shear.number), std::make_shared<Value>(+height.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(-width.number / 2 - shear.number), std::make_shared<Value>(-height.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(+width.number / 2 - shear.number), std::make_shared<Value>(-height.number / 2) }));
	v.push_back(std::make_shared<Value>(tmp));
	vs.push_back(std::make_shared<Value>(v));
	return std::make_shared<Value>(vs);
}
std::shared_ptr<Value> Trapezoid(Value const& topWidth, Value const& bottomWidth, Value const& height)
{
	vector<std::shared_ptr<Value>> vs;
	vector<std::shared_ptr<Value>> v;
	v.push_back(std::make_shared<Value>(0));
	vector<std::shared_ptr<Value>> tmp;
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(+topWidth.number / 2), std::make_shared<Value>(+height.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(-topWidth.number / 2), std::make_shared<Value>(+height.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(-bottomWidth.number / 2), std::make_shared<Value>(-height.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(+bottomWidth.number / 2), std::make_shared<Value>(-height.number / 2) }));
	v.push_back(std::make_shared<Value>(tmp));
	vs.push_back(std::make_shared<Value>(v));
	return std::make_shared<Value>(vs);
}
std::shared_ptr<Value> Cross(Value const& size, Value const& thickness)
{
	vector<std::shared_ptr<Value>> vs;
	vector<std::shared_ptr<Value>> v;
	v.push_back(std::make_shared<Value>(0));
	vector<std::shared_ptr<Value>> tmp;
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(+size.number / 2), std::make_shared<Value>(+thickness.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(-size.number / 2), std::make_shared<Value>(+thickness.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(-size.number / 2), std::make_shared<Value>(-thickness.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(+size.number / 2), std::make_shared<Value>(-thickness.number / 2) }));
	v.push_back(std::make_shared<Value>(tmp));
	vs.push_back(std::make_shared<Value>(v));
	v.clear();
	v.push_back(std::make_shared<Value>(0));
	tmp.clear();
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(+thickness.number / 2), std::make_shared<Value>(+size.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(-thickness.number / 2), std::make_shared<Value>(+size.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(-thickness.number / 2), std::make_shared<Value>(-size.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(+thickness.number / 2), std::make_shared<Value>(-size.number / 2) }));
	v.push_back(std::make_shared<Value>(tmp));
	vs.push_back(std::make_shared<Value>(v));
	return std::make_shared<Value>(vs);
}
std::shared_ptr<Value> FramedBox(Value const& width, Value const& height, Value const& thickness)
{
	Vector2 a0(+width.number / 2 + thickness.number / 2, +height.number / 2 + thickness.number / 2);
	Vector2 a1(-width.number / 2 - thickness.number / 2, +height.number / 2 + thickness.number / 2);
	Vector2 a2(-width.number / 2 - thickness.number / 2, +height.number / 2 - thickness.number / 2);
	Vector2 a3(+width.number / 2 + thickness.number / 2, +height.number / 2 - thickness.number / 2);
	Vector2 b0(+width.number / 2 + thickness.number / 2, -height.number / 2 + thickness.number / 2);
	Vector2 b1(-width.number / 2 - thickness.number / 2, -height.number / 2 + thickness.number / 2);
	Vector2 b2(-width.number / 2 - thickness.number / 2, -height.number / 2 - thickness.number / 2);
	Vector2 b3(+width.number / 2 + thickness.number / 2, -height.number / 2 - thickness.number / 2);
	Vector2 c0(+width.number / 2 + thickness.number / 2, +height.number / 2 + thickness.number / 2);
	Vector2 c1(+width.number / 2 + thickness.number / 2, -height.number / 2 - thickness.number / 2);
	Vector2 c2(+width.number / 2 - thickness.number / 2, -height.number / 2 - thickness.number / 2);
	Vector2 c3(+width.number / 2 - thickness.number / 2, +height.number / 2 + thickness.number / 2);
	Vector2 d0(-width.number / 2 + thickness.number / 2, +height.number / 2 + thickness.number / 2);
	Vector2 d1(-width.number / 2 + thickness.number / 2, -height.number / 2 - thickness.number / 2);
	Vector2 d2(-width.number / 2 - thickness.number / 2, -height.number / 2 - thickness.number / 2);
	Vector2 d3(-width.number / 2 - thickness.number / 2, +height.number / 2 + thickness.number / 2);
	vector<std::shared_ptr<Value>> vs;
	vector<std::shared_ptr<Value>> v;
	v.push_back(std::make_shared<Value>(0));
	vector<std::shared_ptr<Value>> tmp;
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(a0.x), std::make_shared<Value>(a0.y) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(a1.x), std::make_shared<Value>(a1.y) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(a2.x), std::make_shared<Value>(a2.y) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(a3.x), std::make_shared<Value>(a3.y) }));
	v.push_back(std::make_shared<Value>(tmp));
	vs.push_back(std::make_shared<Value>(v));
	v.clear();
	v.push_back(std::make_shared<Value>(0));
	tmp.clear();
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(b0.x), std::make_shared<Value>(b0.y) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(b1.x), std::make_shared<Value>(b1.y) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(b2.x), std::make_shared<Value>(b2.y) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(b3.x), std::make_shared<Value>(b3.y) }));
	v.push_back(std::make_shared<Value>(tmp));
	vs.push_back(std::make_shared<Value>(v));
	v.clear();
	v.push_back(std::make_shared<Value>(0));
	tmp.clear();
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(c3.x), std::make_shared<Value>(c3.y) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(c2.x), std::make_shared<Value>(c2.y) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(c1.x), std::make_shared<Value>(c1.y) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(c0.x), std::make_shared<Value>(c0.y) }));
	v.push_back(std::make_shared<Value>(tmp));
	vs.push_back(std::make_shared<Value>(v));
	v.clear();
	v.push_back(std::make_shared<Value>(0));
	tmp.clear();
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(d3.x), std::make_shared<Value>(d3.y) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(d2.x), std::make_shared<Value>(d2.y) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(d1.x), std::make_shared<Value>(d1.y) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(d0.x), std::make_shared<Value>(d0.y) }));
	v.push_back(std::make_shared<Value>(tmp));
	vs.push_back(std::make_shared<Value>(v));
	return std::make_shared<Value>(vs);
}