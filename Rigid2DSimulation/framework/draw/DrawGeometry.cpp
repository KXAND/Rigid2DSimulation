#include "DrawGeometry.h"
#include "DrawLineSegment.h"

void DrawRectangleRaw(Tile& targetTile, DVector2 topLeft, int width, int height, DRect viewPort, Color color)
{
	int a = max(viewPort.GetLeft(), topLeft.x);
	int b = min(viewPort.GetRight(), topLeft.x + width);
	int c = max(viewPort.GetTop(), topLeft.y);
	int d = min(viewPort.GetBottom(), topLeft.y + height);
	if (a >= b || c >= d) { return; }

	vector<Color> rowColors(b - a, color);
	for (int j = (c), jEnd = int(d); j < jEnd; ++j)
	{
		int position = j * targetTile.width + a;
		memcpy(targetTile.colors.data() + position, rowColors.data(), (b - a) * sizeof(color));
	}
}

void DrawRectangle(Tile& targetTile, DepthBuffer& zBuffer, double depth,
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

void DrawRectangleWithTransform(Tile& targetTile, DepthBuffer& zBuffer, double depth,
	Vector2 center, double width, double height, Matrix2 transformMatrix, DRect viewPort, Color color)
{
	Matrix2 transformInverse = transformMatrix.Inverse();
	Vector2 transformHalfWidthHeight = transformMatrix.Abs() * Vector2(width, height) / 2;
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

void DrawEllipse(Tile& targetTile, DepthBuffer& zBuffer, double depth,
	Vector2 center, double horizontalRadius, double verticalRadius, DRect viewPort, Color color)
{
	int left = max(viewPort.GetLeft(), int(center.x - horizontalRadius) - 1);
	int right = min(viewPort.GetRight(), int(center.x + horizontalRadius) + 1);
	int top = max(viewPort.GetTop(), int(center.y - verticalRadius) - 1);
	int bottom = min(viewPort.GetBottom(), int(center.y + verticalRadius) + 1);
	double inverseHorizontalRadiusSquared = 1 / (horizontalRadius * horizontalRadius);
	double inverseVerticalRadiusSquared = 1 / (verticalRadius * verticalRadius);

	for (int i = (left), iEnd = int(right); i < iEnd; ++i) for (int j = (top), jEnd = int(bottom); j < jEnd; ++j)
	{

		// 应用椭圆的标准方程判断是否在椭圆内
		Vector2 v = Vector2(i, j) - center;
		double normalizedDistanceSquared =
			v.x * v.x * inverseHorizontalRadiusSquared +
			v.y * v.y * inverseVerticalRadiusSquared;
		if (normalizedDistanceSquared < 1)
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

