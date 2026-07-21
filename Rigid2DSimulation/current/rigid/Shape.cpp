#include "Shape.h"

Shape::Shape(double _r, Vector2 _o_rel)
{
	isCircle = true;
	r = _r;
	relativePosition = _o_rel;
}

Shape::Shape(vector<Vector2> const& _verticesRelative)
{
	verticesRelative = _verticesRelative;
	vertices.resize(verticesRelative.size());
}

void Shape::Save(FILE* file) const
{
	int size = 0;
	fwrite(&(isCircle), sizeof(isCircle), 1, file);
	fwrite(&(r), sizeof(r), 1, file);
	fwrite(&(radian), sizeof(radian), 1, file);
	fwrite(&(relativePosition), sizeof(relativePosition), 1, file);
	size = (verticesRelative).size();
	fwrite(&(size), sizeof(size), 1, file);
	fwrite(&*(verticesRelative).begin(), sizeof(*(&*(verticesRelative).begin())), size, file);
}
Shape::Shape(FILE* file)
{
	int size = 0;
	fread(&(isCircle), sizeof(isCircle), 1, file);
	fread(&(r), sizeof(r), 1, file);
	fread(&(radian), sizeof(radian), 1, file);
	fread(&(relativePosition), sizeof(relativePosition), 1, file);
	fread(&(size), sizeof(size), 1, file);
	(verticesRelative).resize(size);
	fread(&*(verticesRelative).begin(), sizeof(*(&*(verticesRelative).begin())), size, file);
	vertices.resize(verticesRelative.size());
}

Vector2 Shape::GenerateRandomPointInside() const
{
	if (isCircle)
	{
		double randomRadian = GenerateRandomFloat(2 * kPi);
		double randomRadius = sqrt(GenerateRandomFloat(1)) * r;
		return Vector2(cos(randomRadian), sin(randomRadian)) * randomRadius;
	}

	double randomArea = GenerateRandomFloat(area);
	for (int i = (0), iEnd = int(vertices.size()); i < iEnd; ++i)
	{
		int j = ((long long) i + 1) % vertices.size();
		auto p = verticesRelative[i];
		auto q = verticesRelative[j];

		// 利用二维空矩阵行列式结果等于两个向量（op，oq）构成的平行四边形的面积得
		randomArea -= Matrix2(p - relativePosition, q - relativePosition).Determinant() / 2;

		// 在第一个令面积为负的区域中随机
		if (randomArea < 0 || i == vertices.size() - 1)
			return GenerateRandomPointInTriangle(relativePosition, p, q);

	}
	return {};
}

bool Shape::IsInside(Vector2 p) const
{
	if (isCircle)
		return (p - position).GetLengthSquared() < r * r;

	for (int i = (0), iEnd = int(vertices.size()); i < iEnd; ++i)
	{
		int j = ((long long) i + 1) % vertices.size();
		if (IsInsideTriangle(p, position, vertices[i], vertices[j]))
			return true;
	}
	return false;
}

Aabb Shape::GetBoundingBox() const
{
	if (isCircle)
		return Aabb(position.x - r, position.x + r, position.y - r, position.y + r);

	Aabb box;
	for (auto v : vertices)
	{
		box.Expand(v);
	}
	return box;
}

Vector2 Shape::GenerateRandomPointInTriangle(Vector2 v0, Vector2 v1, Vector2 v2) const
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

bool Shape::IsInsideTriangle(Vector2 v, Vector2 v0, Vector2 v1, Vector2 v2) const
{
	// 齐次坐标
	Vector3 u(v.x, v.y, 1);
	Vector3 u0(v0.x, v0.y, 1);
	Vector3 u1(v1.x, v1.y, 1);
	Vector3 u2(v2.x, v2.y, 1);
	Vector3 t = Matrix3(u0, u1, u2).Inverse() * u;
	return
		IsBetween(t.x, 0.0, 1.0) &&
		IsBetween(t.y, 0.0, 1.0) &&
		IsBetween(t.z, 0.0, 1.0);
}

vector<Vector2> GetRectVerticesByCenter(double halfWidth, double halfHeight)
{
	vector<Vector2> vertices;
	vertices.push_back(Vector2(+halfWidth, +halfHeight) / 2);
	vertices.push_back(Vector2(-halfWidth, +halfHeight) / 2);
	vertices.push_back(Vector2(-halfWidth, -halfHeight) / 2);
	vertices.push_back(Vector2(+halfWidth, -halfHeight) / 2);
	return vertices;
}

