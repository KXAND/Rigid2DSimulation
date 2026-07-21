#include "ShapeBuiltins.h"
#include "Value.h"
#include "Shape.h"

shared_ptr<Value> Ball(Value const& r)
{
	vector<shared_ptr<Value>> vs;
	vector<shared_ptr<Value>> v;
	v.push_back(make_shared<Value>(1));
	v.push_back(make_shared<Value>(r));
	vs.push_back(make_shared<Value>(v));
	return make_shared<Value>(vs);
}
shared_ptr<Value> Box(Value const& w, Value const& h)
{
	vector<std::shared_ptr<Value>> vs;
	vector<std::shared_ptr<Value>> v;
	v.push_back(std::make_shared<Value>(0));
	vector<std::shared_ptr<Value>> tmp;
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(+w.number / 2), std::make_shared<Value>(+h.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(-w.number / 2), std::make_shared<Value>(+h.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(-w.number / 2), std::make_shared<Value>(-h.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(+w.number / 2), std::make_shared<Value>(-h.number / 2) }));
	v.push_back(std::make_shared<Value>(tmp));
	vs.push_back(std::make_shared<Value>(v));
	return std::make_shared<Value>(vs);
}
std::shared_ptr<Value> RegularPolygon(Value const& n, Value const& r)
{
	vector<std::shared_ptr<Value>> vs;
	vector<std::shared_ptr<Value>> v;
	v.push_back(std::make_shared<Value>(0));
	vector<std::shared_ptr<Value>> tmp;
	auto m = max(3, (int)n.number);
	for (int i = (0), iEnd = int(m); i < iEnd; ++i)
	{
		double phi = 2 * kPi * i / m;
		tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(r.number * cos(phi)), std::make_shared<Value>(r.number * sin(phi)) }));
	}
	v.push_back(std::make_shared<Value>(tmp));
	vs.push_back(std::make_shared<Value>(v));
	return std::make_shared<Value>(vs);
}
std::shared_ptr<Value> Parallelogram(Value const& w, Value const& h, Value const& s)
{
	vector<std::shared_ptr<Value>> vs;
	vector<std::shared_ptr<Value>> v;
	v.push_back(std::make_shared<Value>(0));
	vector<std::shared_ptr<Value>> tmp;
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(+w.number / 2 + s.number), std::make_shared<Value>(+h.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(-w.number / 2 + s.number), std::make_shared<Value>(+h.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(-w.number / 2 - s.number), std::make_shared<Value>(-h.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(+w.number / 2 - s.number), std::make_shared<Value>(-h.number / 2) }));
	v.push_back(std::make_shared<Value>(tmp));
	vs.push_back(std::make_shared<Value>(v));
	return std::make_shared<Value>(vs);
}
std::shared_ptr<Value> Trapezoid(Value const& t, Value const& b, Value const& h)
{
	vector<std::shared_ptr<Value>> vs;
	vector<std::shared_ptr<Value>> v;
	v.push_back(std::make_shared<Value>(0));
	vector<std::shared_ptr<Value>> tmp;
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(+t.number / 2), std::make_shared<Value>(+h.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(-t.number / 2), std::make_shared<Value>(+h.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(-b.number / 2), std::make_shared<Value>(-h.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(+b.number / 2), std::make_shared<Value>(-h.number / 2) }));
	v.push_back(std::make_shared<Value>(tmp));
	vs.push_back(std::make_shared<Value>(v));
	return std::make_shared<Value>(vs);
}
std::shared_ptr<Value> Cross(Value const& s, Value const& thk)
{
	vector<std::shared_ptr<Value>> vs;
	vector<std::shared_ptr<Value>> v;
	v.push_back(std::make_shared<Value>(0));
	vector<std::shared_ptr<Value>> tmp;
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(+s.number / 2), std::make_shared<Value>(+thk.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(-s.number / 2), std::make_shared<Value>(+thk.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(-s.number / 2), std::make_shared<Value>(-thk.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(+s.number / 2), std::make_shared<Value>(-thk.number / 2) }));
	v.push_back(std::make_shared<Value>(tmp));
	vs.push_back(std::make_shared<Value>(v));
	v.clear();
	v.push_back(std::make_shared<Value>(0));
	tmp.clear();
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(+thk.number / 2), std::make_shared<Value>(+s.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(-thk.number / 2), std::make_shared<Value>(+s.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(-thk.number / 2), std::make_shared<Value>(-s.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(+thk.number / 2), std::make_shared<Value>(-s.number / 2) }));
	v.push_back(std::make_shared<Value>(tmp));
	vs.push_back(std::make_shared<Value>(v));
	return std::make_shared<Value>(vs);
}
std::shared_ptr<Value> FramedBox(Value const& w, Value const& h, Value const& thk)
{
	Vector2 a0(+w.number / 2 + thk.number / 2, +h.number / 2 + thk.number / 2);
	Vector2 a1(-w.number / 2 - thk.number / 2, +h.number / 2 + thk.number / 2);
	Vector2 a2(-w.number / 2 - thk.number / 2, +h.number / 2 - thk.number / 2);
	Vector2 a3(+w.number / 2 + thk.number / 2, +h.number / 2 - thk.number / 2);
	Vector2 b0(+w.number / 2 + thk.number / 2, -h.number / 2 + thk.number / 2);
	Vector2 b1(-w.number / 2 - thk.number / 2, -h.number / 2 + thk.number / 2);
	Vector2 b2(-w.number / 2 - thk.number / 2, -h.number / 2 - thk.number / 2);
	Vector2 b3(+w.number / 2 + thk.number / 2, -h.number / 2 - thk.number / 2);
	Vector2 c0(+w.number / 2 + thk.number / 2, +h.number / 2 + thk.number / 2);
	Vector2 c1(+w.number / 2 + thk.number / 2, -h.number / 2 - thk.number / 2);
	Vector2 c2(+w.number / 2 - thk.number / 2, -h.number / 2 - thk.number / 2);
	Vector2 c3(+w.number / 2 - thk.number / 2, +h.number / 2 + thk.number / 2);
	Vector2 d0(-w.number / 2 + thk.number / 2, +h.number / 2 + thk.number / 2);
	Vector2 d1(-w.number / 2 + thk.number / 2, -h.number / 2 - thk.number / 2);
	Vector2 d2(-w.number / 2 - thk.number / 2, -h.number / 2 - thk.number / 2);
	Vector2 d3(-w.number / 2 - thk.number / 2, +h.number / 2 + thk.number / 2);
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