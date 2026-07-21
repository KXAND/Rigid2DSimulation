#include "DrawGeometry.h"
#include "DrawLineSegment.h"

void drawRectangleRaw(Tile& targetTile, DVector2 topLeft, int w, int h, DRect viewPort, Color color)
{
	int a = max(viewPort.GetLeft(), topLeft.x);
	int b = min(viewPort.GetRight(), topLeft.x + w);
	int c = max(viewPort.GetTop(), topLeft.y);
	int d = min(viewPort.GetBottom(), topLeft.y + h);
	if (a >= b || c >= d) { return; }

	vector<Color> tmp(b - a, color);
	for (int j = (c), jEnd = int(d); j < jEnd; ++j)
	{
		int position = j * targetTile.width + a;
		memcpy(targetTile.colors.data() + position, tmp.data(), (b - a) * sizeof(color));
	}
}

void drawRectangle(Tile& targetTile, DepthBuffer& zBuffer, double depth,
	DVector2 topLeft, int width, int height, DRect viewPort, Color color)
{
	int a = max(viewPort.GetLeft(), topLeft.x);
	int b = min(viewPort.GetRight(), topLeft.x + width);
	int c = max(viewPort.GetTop(), topLeft.y);
	int d = min(viewPort.GetBottom(), topLeft.y + height);
	if (a >= b || c >= d) { return; }

	for (int i = (a), iEnd = int(b); i < iEnd; ++i) for (int j = (c), jEnd = int(d); j < jEnd; ++j)
	{
		int position = j * targetTile.width + i;// j 行 i 列
		if (zBuffer[position] <= depth)
		{
			zBuffer[position] = depth;
			targetTile.colors[position] = color;
		}
	}
}

void drawRectangleWithTransform(Tile& targetTile, DepthBuffer& zBuffer, double depth,
	Vector2 center, double width, double height, Matrix2 transformMatrix, DRect viewPort, Color color)
{
	Matrix2 transformInverse = transformMatrix.inverse();
	Vector2 transformHalfWidthHeight = transformMatrix.abs() * Vector2(width, height) / 2;
	// 这个 1 是经验公式。我没做严格论证。
	int a = max(viewPort.GetLeft(), int(center.x - transformHalfWidthHeight.x) - 1);
	int b = min(viewPort.GetRight(), int(center.x + transformHalfWidthHeight.x) + 1);
	int c = max(viewPort.GetTop(), int(center.y - transformHalfWidthHeight.y) - 1);
	int d = min(viewPort.GetBottom(), int(center.y + transformHalfWidthHeight.y) + 1);
	if (a >= b || c >= d) { return; }

	for (int i = (a), iEnd = int(b); i < iEnd; ++i) for (int j = (c), jEnd = int(d); j < jEnd; ++j)
	{
		DVector2 dPoint(i, j);
		// 下面式子的正确性从逆变换比较容易看，要注意四则运算的优先级。
		Vector2 uPoint = Vector2(0.5) +
			transformInverse * (Vector2(dPoint) - center) / Vector2(width, height);
		if (IsInside(uPoint, { 1, 1 }))
		{
			int depthIndex = dPoint.y * targetTile.width + dPoint.x;
			if (zBuffer[depthIndex] <= depth)
			{
				zBuffer[depthIndex] = depth;
				targetTile.colors[depthIndex] = color;
			}
		}
	}
}

void drawEllipse(Tile& targetTile, DepthBuffer& zBuffer, double depth,
	Vector2 center, double a, double b, DRect viewPort, Color color)
{
	int left = max(viewPort.GetLeft(), int(center.x - a) - 1);
	int right = min(viewPort.GetRight(), int(center.x + a) + 1);
	int top = max(viewPort.GetTop(), int(center.y - b) - 1);
	int bottom = min(viewPort.GetBottom(), int(center.y + b) + 1);
	double inv_aax = 1 / (a * a);
	double inv_aay = 1 / (b * b);

	for (int i = (left), iEnd = int(right); i < iEnd; ++i) for (int j = (top), jEnd = int(bottom); j < jEnd; ++j)
	{

		// 应用椭圆的标准方程判断是否在椭圆内
		Vector2 v = Vector2(i, j) - center;
		double tmp =
			v.x * v.x * inv_aax +
			v.y * v.y * inv_aay;
		if (tmp < 1)
		{
			int position = j * targetTile.width + i;
			if (zBuffer[position] <= depth)
			{
				zBuffer[position] = depth;
				targetTile.colors[position] = color;
			}
		}
	}
}

