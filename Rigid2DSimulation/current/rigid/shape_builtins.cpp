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
	vector<ptr<Var>> vs;
	vector<ptr<Var>> v;
	v.push_back(msh<Var>(0));
	vector<ptr<Var>> tmp;
	tmp.push_back(msh<Var>(vector<ptr<Var>>{ msh<Var>(+w.num / 2), msh<Var>(+h.num / 2) }));
	tmp.push_back(msh<Var>(vector<ptr<Var>>{ msh<Var>(-w.num / 2), msh<Var>(+h.num / 2) }));
	tmp.push_back(msh<Var>(vector<ptr<Var>>{ msh<Var>(-w.num / 2), msh<Var>(-h.num / 2) }));
	tmp.push_back(msh<Var>(vector<ptr<Var>>{ msh<Var>(+w.num / 2), msh<Var>(-h.num / 2) }));
	v.push_back(msh<Var>(tmp));
	vs.push_back(msh<Var>(v));
	return msh<Var>(vs);
}
ptr<Var> RegularPoly(Var const& n, Var const& r)
{
	vector<ptr<Var>> vs;
	vector<ptr<Var>> v;
	v.push_back(msh<Var>(0));
	vector<ptr<Var>> tmp;
	auto m = max(3, (int)n.num);
	for (int i = (0), iEnd = int(m); i < iEnd; ++i)
	{
		double phi = 2 * PI * i / m;
		tmp.push_back(msh<Var>(vector<ptr<Var>>{ msh<Var>(r.num * cos(phi)), msh<Var>(r.num * sin(phi)) }));
	}
	v.push_back(msh<Var>(tmp));
	vs.push_back(msh<Var>(v));
	return msh<Var>(vs);
}
ptr<Var> Parallelogram(Var const& w, Var const& h, Var const& s)
{
	vector<ptr<Var>> vs;
	vector<ptr<Var>> v;
	v.push_back(msh<Var>(0));
	vector<ptr<Var>> tmp;
	tmp.push_back(msh<Var>(vector<ptr<Var>>{ msh<Var>(+w.num / 2 + s.num), msh<Var>(+h.num / 2) }));
	tmp.push_back(msh<Var>(vector<ptr<Var>>{ msh<Var>(-w.num / 2 + s.num), msh<Var>(+h.num / 2) }));
	tmp.push_back(msh<Var>(vector<ptr<Var>>{ msh<Var>(-w.num / 2 - s.num), msh<Var>(-h.num / 2) }));
	tmp.push_back(msh<Var>(vector<ptr<Var>>{ msh<Var>(+w.num / 2 - s.num), msh<Var>(-h.num / 2) }));
	v.push_back(msh<Var>(tmp));
	vs.push_back(msh<Var>(v));
	return msh<Var>(vs);
}
ptr<Var> Trapezoid(Var const& t, Var const& b, Var const& h)
{
	vector<ptr<Var>> vs;
	vector<ptr<Var>> v;
	v.push_back(msh<Var>(0));
	vector<ptr<Var>> tmp;
	tmp.push_back(msh<Var>(vector<ptr<Var>>{ msh<Var>(+t.num / 2), msh<Var>(+h.num / 2) }));
	tmp.push_back(msh<Var>(vector<ptr<Var>>{ msh<Var>(-t.num / 2), msh<Var>(+h.num / 2) }));
	tmp.push_back(msh<Var>(vector<ptr<Var>>{ msh<Var>(-b.num / 2), msh<Var>(-h.num / 2) }));
	tmp.push_back(msh<Var>(vector<ptr<Var>>{ msh<Var>(+b.num / 2), msh<Var>(-h.num / 2) }));
	v.push_back(msh<Var>(tmp));
	vs.push_back(msh<Var>(v));
	return msh<Var>(vs);
}
ptr<Var> Cross(Var const& s, Var const& thk)
{
	vector<ptr<Var>> vs;
	vector<ptr<Var>> v;
	v.push_back(msh<Var>(0));
	vector<ptr<Var>> tmp;
	tmp.push_back(msh<Var>(vector<ptr<Var>>{ msh<Var>(+s.num / 2), msh<Var>(+thk.num / 2) }));
	tmp.push_back(msh<Var>(vector<ptr<Var>>{ msh<Var>(-s.num / 2), msh<Var>(+thk.num / 2) }));
	tmp.push_back(msh<Var>(vector<ptr<Var>>{ msh<Var>(-s.num / 2), msh<Var>(-thk.num / 2) }));
	tmp.push_back(msh<Var>(vector<ptr<Var>>{ msh<Var>(+s.num / 2), msh<Var>(-thk.num / 2) }));
	v.push_back(msh<Var>(tmp));
	vs.push_back(msh<Var>(v));
	v.clear();
	v.push_back(msh<Var>(0));
	tmp.clear();
	tmp.push_back(msh<Var>(vector<ptr<Var>>{ msh<Var>(+thk.num / 2), msh<Var>(+s.num / 2) }));
	tmp.push_back(msh<Var>(vector<ptr<Var>>{ msh<Var>(-thk.num / 2), msh<Var>(+s.num / 2) }));
	tmp.push_back(msh<Var>(vector<ptr<Var>>{ msh<Var>(-thk.num / 2), msh<Var>(-s.num / 2) }));
	tmp.push_back(msh<Var>(vector<ptr<Var>>{ msh<Var>(+thk.num / 2), msh<Var>(-s.num / 2) }));
	v.push_back(msh<Var>(tmp));
	vs.push_back(msh<Var>(v));
	return msh<Var>(vs);
}
ptr<Var> FramedBox(Var const& w, Var const& h, Var const& thk)
{
	vector2 a0(+w.num / 2 + thk.num / 2, +h.num / 2 + thk.num / 2);
	vector2 a1(-w.num / 2 - thk.num / 2, +h.num / 2 + thk.num / 2);
	vector2 a2(-w.num / 2 - thk.num / 2, +h.num / 2 - thk.num / 2);
	vector2 a3(+w.num / 2 + thk.num / 2, +h.num / 2 - thk.num / 2);
	vector2 b0(+w.num / 2 + thk.num / 2, -h.num / 2 + thk.num / 2);
	vector2 b1(-w.num / 2 - thk.num / 2, -h.num / 2 + thk.num / 2);
	vector2 b2(-w.num / 2 - thk.num / 2, -h.num / 2 - thk.num / 2);
	vector2 b3(+w.num / 2 + thk.num / 2, -h.num / 2 - thk.num / 2);
	vector2 c0(+w.num / 2 + thk.num / 2, +h.num / 2 + thk.num / 2);
	vector2 c1(+w.num / 2 + thk.num / 2, -h.num / 2 - thk.num / 2);
	vector2 c2(+w.num / 2 - thk.num / 2, -h.num / 2 - thk.num / 2);
	vector2 c3(+w.num / 2 - thk.num / 2, +h.num / 2 + thk.num / 2);
	vector2 d0(-w.num / 2 + thk.num / 2, +h.num / 2 + thk.num / 2);
	vector2 d1(-w.num / 2 + thk.num / 2, -h.num / 2 - thk.num / 2);
	vector2 d2(-w.num / 2 - thk.num / 2, -h.num / 2 - thk.num / 2);
	vector2 d3(-w.num / 2 - thk.num / 2, +h.num / 2 + thk.num / 2);
	vector<ptr<Var>> vs;
	vector<ptr<Var>> v;
	v.push_back(msh<Var>(0));
	vector<ptr<Var>> tmp;
	tmp.push_back(msh<Var>(vector<ptr<Var>>{ msh<Var>(a0.x), msh<Var>(a0.y) }));
	tmp.push_back(msh<Var>(vector<ptr<Var>>{ msh<Var>(a1.x), msh<Var>(a1.y) }));
	tmp.push_back(msh<Var>(vector<ptr<Var>>{ msh<Var>(a2.x), msh<Var>(a2.y) }));
	tmp.push_back(msh<Var>(vector<ptr<Var>>{ msh<Var>(a3.x), msh<Var>(a3.y) }));
	v.push_back(msh<Var>(tmp));
	vs.push_back(msh<Var>(v));
	v.clear();
	v.push_back(msh<Var>(0));
	tmp.clear();
	tmp.push_back(msh<Var>(vector<ptr<Var>>{ msh<Var>(b0.x), msh<Var>(b0.y) }));
	tmp.push_back(msh<Var>(vector<ptr<Var>>{ msh<Var>(b1.x), msh<Var>(b1.y) }));
	tmp.push_back(msh<Var>(vector<ptr<Var>>{ msh<Var>(b2.x), msh<Var>(b2.y) }));
	tmp.push_back(msh<Var>(vector<ptr<Var>>{ msh<Var>(b3.x), msh<Var>(b3.y) }));
	v.push_back(msh<Var>(tmp));
	vs.push_back(msh<Var>(v));
	v.clear();
	v.push_back(msh<Var>(0));
	tmp.clear();
	tmp.push_back(msh<Var>(vector<ptr<Var>>{ msh<Var>(c3.x), msh<Var>(c3.y) }));
	tmp.push_back(msh<Var>(vector<ptr<Var>>{ msh<Var>(c2.x), msh<Var>(c2.y) }));
	tmp.push_back(msh<Var>(vector<ptr<Var>>{ msh<Var>(c1.x), msh<Var>(c1.y) }));
	tmp.push_back(msh<Var>(vector<ptr<Var>>{ msh<Var>(c0.x), msh<Var>(c0.y) }));
	v.push_back(msh<Var>(tmp));
	vs.push_back(msh<Var>(v));
	v.clear();
	v.push_back(msh<Var>(0));
	tmp.clear();
	tmp.push_back(msh<Var>(vector<ptr<Var>>{ msh<Var>(d3.x), msh<Var>(d3.y) }));
	tmp.push_back(msh<Var>(vector<ptr<Var>>{ msh<Var>(d2.x), msh<Var>(d2.y) }));
	tmp.push_back(msh<Var>(vector<ptr<Var>>{ msh<Var>(d1.x), msh<Var>(d1.y) }));
	tmp.push_back(msh<Var>(vector<ptr<Var>>{ msh<Var>(d0.x), msh<Var>(d0.y) }));
	v.push_back(msh<Var>(tmp));
	vs.push_back(msh<Var>(v));
	return msh<Var>(vs);
}