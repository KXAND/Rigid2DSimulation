#include "shape.h"

Shape::Shape(double _r, vector2 _o_rel)
{
	isCircle = true;
	r = _r;
	oRelative = _o_rel;
}

Shape::Shape(vector<vector2> const& _verticesRelative)
{
	verticesRelative = _verticesRelative;
	vertices.resize(verticesRelative.size());
}

void Shape::Save(FILE* f) const
{
	int sz = 0;
	fwt(isCircle); fwt(r); fwt(radian); fwt(oRelative);
	fwtv(verticesRelative);
}
Shape::Shape(FILE* f)
{
	int sz = 0;
	frd(isCircle); frd(r); frd(radian); frd(oRelative);
	frdv(verticesRelative); vertices.resize(verticesRelative.size());
}

vector2 Shape::generateRandomPointInside() const
{
	if (isCircle)
	{
		double randomRadian = generateRadomFloat(2 * PI);
		double randomRadius = sqrt(generateRadomFloat(1)) * r;
		return vector2(cos(randomRadian), sin(randomRadian)) * randomRadius;
	}

	double randomArea = generateRadomFloat(area);
	rep(i, 0, vertices.size())
	{
		int j = ((long long) i + 1) % vertices.size();
		auto p = verticesRelative[i];
		auto q = verticesRelative[j];

		// 利用二维空矩阵行列式结果等于两个向量（op，oq）构成的平行四边形的面积得
		randomArea -= matrix2(p - oRelative, q - oRelative).det() / 2;

		// 在第一个令面积为负的区域中随机
		if (randomArea < 0 || i == vertices.size() - 1)
			return generateRadomPointInTriangle(oRelative, p, q);

	}
	return {};
}

bool Shape::isInside(vector2 p) const
{
	if (isCircle)
		return (p - o).lensqr() < r * r;

	rep(i, 0, vertices.size())
	{
		int j = ((long long) i + 1) % vertices.size();
		if (isInsideTriangle(p, o, vertices[i], vertices[j]))
			return true;
	}
	return false;
}

aabb Shape::getBoundingBox() const
{
	if (isCircle)
		return aabb(o.x - r, o.x + r, o.y - r, o.y + r);

	aabb box;
	for (auto v : vertices)
	{
		box.expand(v);
	}
	return box;
}

vector2 Shape::generateRadomPointInTriangle(vector2 v0, vector2 v1, vector2 v2) const
{
	double x0 = generateRadomFloat(1);
	double x1 = generateRadomFloat(1);
	if (x0 + x1 > 1)
	{
		// @ques：这里为什么交换值不是很懂
		swap(x0, x1);
		x0 = 1 - x0;
		x1 = 1 - x1;
	}
	return x0 * v0 + x1 * v1 + (1 - x0 - x1) * v2;
}

bool Shape::isInsideTriangle(vector2 v, vector2 v0, vector2 v1, vector2 v2) const
{
	// 齐次坐标
	vector3 u(v.x, v.y, 1);
	vector3 u0(v0.x, v0.y, 1);
	vector3 u1(v1.x, v1.y, 1);
	vector3 u2(v2.x, v2.y, 1);
	vector3 t = matrix3(u0, u1, u2).inverse() * u;
	return
		isBetween(t.x, 0.0, 1.0) &&
		isBetween(t.y, 0.0, 1.0) &&
		isBetween(t.z, 0.0, 1.0);
}

vector<vector2> getRectVerticesByCenter(double halfWidth, double halfHeight)
{
	vector<vector2> vertices;
	vertices.push_back(vector2(+halfWidth, +halfHeight) / 2);
	vertices.push_back(vector2(-halfWidth, +halfHeight) / 2);
	vertices.push_back(vector2(-halfWidth, -halfHeight) / 2);
	vertices.push_back(vector2(+halfWidth, -halfHeight) / 2);
	return vertices;
}
