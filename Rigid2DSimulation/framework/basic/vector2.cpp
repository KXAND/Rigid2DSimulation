#include "vector2.h"

vector2::vector2(wstring const& s)
{
	int i = 0;
	x = fetch_num(i, s);
	y = fetch_num(i, s);
}
wstring toWstring(vector2 v)
{
	return L"(" + tw2(v.x) + L"," + tw2(v.y) + L")";
}

bool isInside(vector2 v, rect r)
{
	return
		isBetween(v.x, r.left(), r.right()) &&
		isBetween(v.y, r.top(), r.bottom());
}
/// <summary>
/// �ݹ�����ȡ����α����������ϵĵ�
/// </summary>
/// <param name="controlPoints">���Ƶ㼯</param>
/// <param name="t">[0,1]��������Ӧ�Ĳ���</param>
/// <returns></returns>
vector2 getPositionOnBezier(vector<vector2> controlPoints, double t)
{
// @todo����֪���ܲ����Ż���ͬʱ���κ�����Ӧ���е�����ʵ�֡�
	if (controlPoints.size() == 2) { return controlPoints[0] * (1 - t) + controlPoints[1] * t; }

	vector<vector2> ps0, ps1;
	rep(i, 0, controlPoints.size())
	{
		if (i != 0) { ps1.push_back(controlPoints[i]); }
		if (i != controlPoints.size() - 1) { ps0.push_back(controlPoints[i]); }
	}
	return getPositionOnBezier(ps0, t) * (1 - t) + getPositionOnBezier(ps1, t) * t;
}

double getDistanceSquare(vector2 a, vector2 b, vector2 p)
{
	if (a == b) { return (a - p).lensqr(); }

	double ap = (p - a).lensqr();
	double bp = (p - b).lensqr();
	double ab = (a - b).lensqr();
	double diff = ap - bp;
	return
		ab + bp < ap ? bp : // bp ̫�̣��޷����������Σ���ͬ
		ab + ap < bp ? ap :
		(ap + bp) / 2 - ab / 4 - diff * diff / (4 * ab);// @todo����ϰ�Ƶ�
}

