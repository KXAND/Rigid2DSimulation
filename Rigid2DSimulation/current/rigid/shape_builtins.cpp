#include "shape_builtins.h"
#include "var.h"
#include "shape.h"

shared_ptr<Var> Ball(Var const& r)
{
	vector<shared_ptr<Var>> vs;
	vector<shared_ptr<Var>> v;
	v.push_back(make_shared<Var>(1));
	v.push_back(make_shared<Var>(r));
	vs.push_back(make_shared<Var>(v));
	return make_shared<Var>(vs);
}
shared_ptr<Var> Box(Var const& w, Var const& h)
{
	vector<std::shared_ptr<Var>> vs;
	vector<std::shared_ptr<Var>> v;
	v.push_back(std::make_shared<Var>(0));
	vector<std::shared_ptr<Var>> tmp;
	tmp.push_back(std::make_shared<Var>(vector<std::shared_ptr<Var>>{ std::make_shared<Var>(+w.num / 2), std::make_shared<Var>(+h.num / 2) }));
	tmp.push_back(std::make_shared<Var>(vector<std::shared_ptr<Var>>{ std::make_shared<Var>(-w.num / 2), std::make_shared<Var>(+h.num / 2) }));
	tmp.push_back(std::make_shared<Var>(vector<std::shared_ptr<Var>>{ std::make_shared<Var>(-w.num / 2), std::make_shared<Var>(-h.num / 2) }));
	tmp.push_back(std::make_shared<Var>(vector<std::shared_ptr<Var>>{ std::make_shared<Var>(+w.num / 2), std::make_shared<Var>(-h.num / 2) }));
	v.push_back(std::make_shared<Var>(tmp));
	vs.push_back(std::make_shared<Var>(v));
	return std::make_shared<Var>(vs);
}
std::shared_ptr<Var> RegularPoly(Var const& n, Var const& r)
{
	vector<std::shared_ptr<Var>> vs;
	vector<std::shared_ptr<Var>> v;
	v.push_back(std::make_shared<Var>(0));
	vector<std::shared_ptr<Var>> tmp;
	auto m = max(3, (int)n.num);
	for (int i = (0), iEnd = int(m); i < iEnd; ++i)
	{
		double phi = 2 * PI * i / m;
		tmp.push_back(std::make_shared<Var>(vector<std::shared_ptr<Var>>{ std::make_shared<Var>(r.num * cos(phi)), std::make_shared<Var>(r.num * sin(phi)) }));
	}
	v.push_back(std::make_shared<Var>(tmp));
	vs.push_back(std::make_shared<Var>(v));
	return std::make_shared<Var>(vs);
}
std::shared_ptr<Var> Parallelogram(Var const& w, Var const& h, Var const& s)
{
	vector<std::shared_ptr<Var>> vs;
	vector<std::shared_ptr<Var>> v;
	v.push_back(std::make_shared<Var>(0));
	vector<std::shared_ptr<Var>> tmp;
	tmp.push_back(std::make_shared<Var>(vector<std::shared_ptr<Var>>{ std::make_shared<Var>(+w.num / 2 + s.num), std::make_shared<Var>(+h.num / 2) }));
	tmp.push_back(std::make_shared<Var>(vector<std::shared_ptr<Var>>{ std::make_shared<Var>(-w.num / 2 + s.num), std::make_shared<Var>(+h.num / 2) }));
	tmp.push_back(std::make_shared<Var>(vector<std::shared_ptr<Var>>{ std::make_shared<Var>(-w.num / 2 - s.num), std::make_shared<Var>(-h.num / 2) }));
	tmp.push_back(std::make_shared<Var>(vector<std::shared_ptr<Var>>{ std::make_shared<Var>(+w.num / 2 - s.num), std::make_shared<Var>(-h.num / 2) }));
	v.push_back(std::make_shared<Var>(tmp));
	vs.push_back(std::make_shared<Var>(v));
	return std::make_shared<Var>(vs);
}
std::shared_ptr<Var> Trapezoid(Var const& t, Var const& b, Var const& h)
{
	vector<std::shared_ptr<Var>> vs;
	vector<std::shared_ptr<Var>> v;
	v.push_back(std::make_shared<Var>(0));
	vector<std::shared_ptr<Var>> tmp;
	tmp.push_back(std::make_shared<Var>(vector<std::shared_ptr<Var>>{ std::make_shared<Var>(+t.num / 2), std::make_shared<Var>(+h.num / 2) }));
	tmp.push_back(std::make_shared<Var>(vector<std::shared_ptr<Var>>{ std::make_shared<Var>(-t.num / 2), std::make_shared<Var>(+h.num / 2) }));
	tmp.push_back(std::make_shared<Var>(vector<std::shared_ptr<Var>>{ std::make_shared<Var>(-b.num / 2), std::make_shared<Var>(-h.num / 2) }));
	tmp.push_back(std::make_shared<Var>(vector<std::shared_ptr<Var>>{ std::make_shared<Var>(+b.num / 2), std::make_shared<Var>(-h.num / 2) }));
	v.push_back(std::make_shared<Var>(tmp));
	vs.push_back(std::make_shared<Var>(v));
	return std::make_shared<Var>(vs);
}
std::shared_ptr<Var> Cross(Var const& s, Var const& thk)
{
	vector<std::shared_ptr<Var>> vs;
	vector<std::shared_ptr<Var>> v;
	v.push_back(std::make_shared<Var>(0));
	vector<std::shared_ptr<Var>> tmp;
	tmp.push_back(std::make_shared<Var>(vector<std::shared_ptr<Var>>{ std::make_shared<Var>(+s.num / 2), std::make_shared<Var>(+thk.num / 2) }));
	tmp.push_back(std::make_shared<Var>(vector<std::shared_ptr<Var>>{ std::make_shared<Var>(-s.num / 2), std::make_shared<Var>(+thk.num / 2) }));
	tmp.push_back(std::make_shared<Var>(vector<std::shared_ptr<Var>>{ std::make_shared<Var>(-s.num / 2), std::make_shared<Var>(-thk.num / 2) }));
	tmp.push_back(std::make_shared<Var>(vector<std::shared_ptr<Var>>{ std::make_shared<Var>(+s.num / 2), std::make_shared<Var>(-thk.num / 2) }));
	v.push_back(std::make_shared<Var>(tmp));
	vs.push_back(std::make_shared<Var>(v));
	v.clear();
	v.push_back(std::make_shared<Var>(0));
	tmp.clear();
	tmp.push_back(std::make_shared<Var>(vector<std::shared_ptr<Var>>{ std::make_shared<Var>(+thk.num / 2), std::make_shared<Var>(+s.num / 2) }));
	tmp.push_back(std::make_shared<Var>(vector<std::shared_ptr<Var>>{ std::make_shared<Var>(-thk.num / 2), std::make_shared<Var>(+s.num / 2) }));
	tmp.push_back(std::make_shared<Var>(vector<std::shared_ptr<Var>>{ std::make_shared<Var>(-thk.num / 2), std::make_shared<Var>(-s.num / 2) }));
	tmp.push_back(std::make_shared<Var>(vector<std::shared_ptr<Var>>{ std::make_shared<Var>(+thk.num / 2), std::make_shared<Var>(-s.num / 2) }));
	v.push_back(std::make_shared<Var>(tmp));
	vs.push_back(std::make_shared<Var>(v));
	return std::make_shared<Var>(vs);
}
std::shared_ptr<Var> FramedBox(Var const& w, Var const& h, Var const& thk)
{
	Vector2 a0(+w.num / 2 + thk.num / 2, +h.num / 2 + thk.num / 2);
	Vector2 a1(-w.num / 2 - thk.num / 2, +h.num / 2 + thk.num / 2);
	Vector2 a2(-w.num / 2 - thk.num / 2, +h.num / 2 - thk.num / 2);
	Vector2 a3(+w.num / 2 + thk.num / 2, +h.num / 2 - thk.num / 2);
	Vector2 b0(+w.num / 2 + thk.num / 2, -h.num / 2 + thk.num / 2);
	Vector2 b1(-w.num / 2 - thk.num / 2, -h.num / 2 + thk.num / 2);
	Vector2 b2(-w.num / 2 - thk.num / 2, -h.num / 2 - thk.num / 2);
	Vector2 b3(+w.num / 2 + thk.num / 2, -h.num / 2 - thk.num / 2);
	Vector2 c0(+w.num / 2 + thk.num / 2, +h.num / 2 + thk.num / 2);
	Vector2 c1(+w.num / 2 + thk.num / 2, -h.num / 2 - thk.num / 2);
	Vector2 c2(+w.num / 2 - thk.num / 2, -h.num / 2 - thk.num / 2);
	Vector2 c3(+w.num / 2 - thk.num / 2, +h.num / 2 + thk.num / 2);
	Vector2 d0(-w.num / 2 + thk.num / 2, +h.num / 2 + thk.num / 2);
	Vector2 d1(-w.num / 2 + thk.num / 2, -h.num / 2 - thk.num / 2);
	Vector2 d2(-w.num / 2 - thk.num / 2, -h.num / 2 - thk.num / 2);
	Vector2 d3(-w.num / 2 - thk.num / 2, +h.num / 2 + thk.num / 2);
	vector<std::shared_ptr<Var>> vs;
	vector<std::shared_ptr<Var>> v;
	v.push_back(std::make_shared<Var>(0));
	vector<std::shared_ptr<Var>> tmp;
	tmp.push_back(std::make_shared<Var>(vector<std::shared_ptr<Var>>{ std::make_shared<Var>(a0.x), std::make_shared<Var>(a0.y) }));
	tmp.push_back(std::make_shared<Var>(vector<std::shared_ptr<Var>>{ std::make_shared<Var>(a1.x), std::make_shared<Var>(a1.y) }));
	tmp.push_back(std::make_shared<Var>(vector<std::shared_ptr<Var>>{ std::make_shared<Var>(a2.x), std::make_shared<Var>(a2.y) }));
	tmp.push_back(std::make_shared<Var>(vector<std::shared_ptr<Var>>{ std::make_shared<Var>(a3.x), std::make_shared<Var>(a3.y) }));
	v.push_back(std::make_shared<Var>(tmp));
	vs.push_back(std::make_shared<Var>(v));
	v.clear();
	v.push_back(std::make_shared<Var>(0));
	tmp.clear();
	tmp.push_back(std::make_shared<Var>(vector<std::shared_ptr<Var>>{ std::make_shared<Var>(b0.x), std::make_shared<Var>(b0.y) }));
	tmp.push_back(std::make_shared<Var>(vector<std::shared_ptr<Var>>{ std::make_shared<Var>(b1.x), std::make_shared<Var>(b1.y) }));
	tmp.push_back(std::make_shared<Var>(vector<std::shared_ptr<Var>>{ std::make_shared<Var>(b2.x), std::make_shared<Var>(b2.y) }));
	tmp.push_back(std::make_shared<Var>(vector<std::shared_ptr<Var>>{ std::make_shared<Var>(b3.x), std::make_shared<Var>(b3.y) }));
	v.push_back(std::make_shared<Var>(tmp));
	vs.push_back(std::make_shared<Var>(v));
	v.clear();
	v.push_back(std::make_shared<Var>(0));
	tmp.clear();
	tmp.push_back(std::make_shared<Var>(vector<std::shared_ptr<Var>>{ std::make_shared<Var>(c3.x), std::make_shared<Var>(c3.y) }));
	tmp.push_back(std::make_shared<Var>(vector<std::shared_ptr<Var>>{ std::make_shared<Var>(c2.x), std::make_shared<Var>(c2.y) }));
	tmp.push_back(std::make_shared<Var>(vector<std::shared_ptr<Var>>{ std::make_shared<Var>(c1.x), std::make_shared<Var>(c1.y) }));
	tmp.push_back(std::make_shared<Var>(vector<std::shared_ptr<Var>>{ std::make_shared<Var>(c0.x), std::make_shared<Var>(c0.y) }));
	v.push_back(std::make_shared<Var>(tmp));
	vs.push_back(std::make_shared<Var>(v));
	v.clear();
	v.push_back(std::make_shared<Var>(0));
	tmp.clear();
	tmp.push_back(std::make_shared<Var>(vector<std::shared_ptr<Var>>{ std::make_shared<Var>(d3.x), std::make_shared<Var>(d3.y) }));
	tmp.push_back(std::make_shared<Var>(vector<std::shared_ptr<Var>>{ std::make_shared<Var>(d2.x), std::make_shared<Var>(d2.y) }));
	tmp.push_back(std::make_shared<Var>(vector<std::shared_ptr<Var>>{ std::make_shared<Var>(d1.x), std::make_shared<Var>(d1.y) }));
	tmp.push_back(std::make_shared<Var>(vector<std::shared_ptr<Var>>{ std::make_shared<Var>(d0.x), std::make_shared<Var>(d0.y) }));
	v.push_back(std::make_shared<Var>(tmp));
	vs.push_back(std::make_shared<Var>(v));
	return std::make_shared<Var>(vs);
}