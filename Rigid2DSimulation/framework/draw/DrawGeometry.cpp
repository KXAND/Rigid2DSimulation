#include "DrawGeometry.h"
#include "DrawLineSegment.h"

void DrawRectangleRaw(Tile& targetTile, DVector2 topLeft, int width, int height, DRect viewport, Color color)
{
	int left = max(viewport.GetLeft(), topLeft.x);
	int right = min(viewport.GetRight(), topLeft.x + width);
	int top = max(viewport.GetTop(), topLeft.y);
	int bottom = min(viewport.GetBottom(), topLeft.y + height);
	if (left >= right || top >= bottom) { return; }

	vector<Color> rowColors(right - left, color);
	for (int j = (top), jEnd = int(bottom); j < jEnd; ++j)
	{
		int position = j * targetTile.width + left;
		memcpy(targetTile.colors.data() + position, rowColors.data(), (right - left) * sizeof(color));
	}
}

void DrawRectangle(Tile& targetTile, DepthBuffer& zBuffer, double depth,
	DVector2 topLeft, int width, int height, DRect viewport, Color color)
{
	int left = max(viewport.GetLeft(), topLeft.x);
	int right = min(viewport.GetRight(), topLeft.x + width);
	int top = max(viewport.GetTop(), topLeft.y);
	int bottom = min(viewport.GetBottom(), topLeft.y + height);
	if (left >= right || top >= bottom) { return; }

	for (int i = (left), iEnd = int(right); i < iEnd; ++i) for (int j = (top), jEnd = int(bottom); j < jEnd; ++j)
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
	Vector2 center, double width, double height, Matrix2 transformMatrix, DRect viewport, Color color)
{
	Matrix2 transformInverse = transformMatrix.Inverse();
	Vector2 transformHalfWidthHeight = transformMatrix.Abs() * Vector2(width, height) / 2;
	// 这个 1 是经验公式。我没做严格论证。
	int left = max(viewport.GetLeft(), int(center.x - transformHalfWidthHeight.x) - 1);
	int right = min(viewport.GetRight(), int(center.x + transformHalfWidthHeight.x) + 1);
	int top = max(viewport.GetTop(), int(center.y - transformHalfWidthHeight.y) - 1);
	int bottom = min(viewport.GetBottom(), int(center.y + transformHalfWidthHeight.y) + 1);
	if (left >= right || top >= bottom) { return; }

	for (int i = (left), iEnd = int(right); i < iEnd; ++i) for (int j = (top), jEnd = int(bottom); j < jEnd; ++j)
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
	Vector2 center, double horizontalRadius, double verticalRadius, DRect viewport, Color color)
{
	int left = max(viewport.GetLeft(), int(center.x - horizontalRadius) - 1);
	int right = min(viewport.GetRight(), int(center.x + horizontalRadius) + 1);
	int top = max(viewport.GetTop(), int(center.y - verticalRadius) - 1);
	int bottom = min(viewport.GetBottom(), int(center.y + verticalRadius) + 1);
	double inverseHorizontalRadiusSquared = 1 / (horizontalRadius * horizontalRadius);
	double inverseVerticalRadiusSquared = 1 / (verticalRadius * verticalRadius);

	for (int i = (left), iEnd = int(right); i < iEnd; ++i) for (int j = (top), jEnd = int(bottom); j < jEnd; ++j)
	{

		// 应用椭圆的标准方程判断是否在椭圆内
		Vector2 offset = Vector2(i, j) - center;
		double normalizedDistanceSquared =
			offset.x * offset.x * inverseHorizontalRadiusSquared +
			offset.y * offset.y * inverseVerticalRadiusSquared;
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

