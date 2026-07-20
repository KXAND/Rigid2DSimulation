#include "shape.h"

Shape::Shape(double _r, vector2 _o_rel)
{
	isCircle = true;
	r = _r;
	relativePosition = _o_rel;
}

Shape::Shape(vector<vector2> const& _verticesRelative)
{
	verticesRelative = _verticesRelative;
	vertices.resize(verticesRelative.size());
}

void Shape::Save(FILE* f) const
{
	int sz = 0;
	fwrite(&(isCircle), sizeof(isCircle), 1, f);
	fwrite(&(r), sizeof(r), 1, f);
	fwrite(&(radian), sizeof(radian), 1, f);
	fwrite(&(relativePosition), sizeof(relativePosition), 1, f);
	sz = (verticesRelative).size();
	fwrite(&(sz), sizeof(sz), 1, f);
	fwrite(&*(verticesRelative).begin(), sizeof(*(&*(verticesRelative).begin())), sz, f);
}
Shape::Shape(FILE* f)
{
	int sz = 0;
	fread(&(isCircle), sizeof(isCircle), 1, f);
	fread(&(r), sizeof(r), 1, f);
	fread(&(radian), sizeof(radian), 1, f);
	fread(&(relativePosition), sizeof(relativePosition), 1, f);
	fread(&(sz), sizeof(sz), 1, f);
	(verticesRelative).resize(sz);
	fread(&*(verticesRelative).begin(), sizeof(*(&*(verticesRelative).begin())), sz, f);
	vertices.resize(verticesRelative.size());
}

vector2 Shape::GenerateRandomPointInside() const
{
	if (isCircle)
	{
		double randomRadian = GenerateRandomFloat(2 * PI);
		double randomRadius = sqrt(GenerateRandomFloat(1)) * r;
		return vector2(cos(randomRadian), sin(randomRadian)) * randomRadius;
	}

	double randomArea = GenerateRandomFloat(area);
	for (int i = (0), iEnd = int(vertices.size()); i < iEnd; ++i)
	{
		int j = ((long long) i + 1) % vertices.size();
		auto p = verticesRelative[i];
		auto q = verticesRelative[j];

		// 利用二维空矩阵行列式结果等于两个向量（op，oq）构成的平行四边形的面积得
		randomArea -= matrix2(p - relativePosition, q - relativePosition).det() / 2;

		// 在第一个令面积为负的区域中随机
		if (randomArea < 0 || i == vertices.size() - 1)
			return GenerateRandomPointInTriangle(relativePosition, p, q);

	}
	return {};
}

bool Shape::IsInside(vector2 p) const
{
	if (isCircle)
		return (p - position).lensqr() < r * r;

	for (int i = (0), iEnd = int(vertices.size()); i < iEnd; ++i)
	{
		int j = ((long long) i + 1) % vertices.size();
		if (IsInsideTriangle(p, position, vertices[i], vertices[j]))
			return true;
	}
	return false;
}

aabb Shape::GetBoundingBox() const
{
	if (isCircle)
		return aabb(position.x - r, position.x + r, position.y - r, position.y + r);

	aabb box;
	for (auto v : vertices)
	{
		box.Expand(v);
	}
	return box;
}

vector2 Shape::GenerateRandomPointInTriangle(vector2 v0, vector2 v1, vector2 v2) const
{
	double x0 = GenerateRandomFloat(1);
	double x1 = GenerateRandomFloat(1);
	if (x0 + x1 > 1)
	{
		// @ques：这里为什么交换值不是很懂
		swap(x0, x1);
		x0 = 1 - x0;
		x1 = 1 - x1;
	}
	return x0 * v0 + x1 * v1 + (1 - x0 - x1) * v2;
}

bool Shape::IsInsideTriangle(vector2 v, vector2 v0, vector2 v1, vector2 v2) const
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

vector<vector2> GetRectVerticesByCenter(double halfWidth, double halfHeight)
{
	vector<vector2> vertices;
	vertices.push_back(vector2(+halfWidth, +halfHeight) / 2);
	vertices.push_back(vector2(-halfWidth, +halfHeight) / 2);
	vertices.push_back(vector2(-halfWidth, -halfHeight) / 2);
	vertices.push_back(vector2(+halfWidth, -halfHeight) / 2);
	return vertices;
}

