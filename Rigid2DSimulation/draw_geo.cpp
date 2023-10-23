#include "draw_geo.h"
#include "draw_px_seg.h"

void drawRectangleRaw(tile& targetTile, dVector2 topLeft, int w, int h, dRect viewPort, dColor color) {
	int a = max(viewPort.left(), topLeft.x);
	int b = min(viewPort.right(), topLeft.x + w);
	int c = max(viewPort.top(), topLeft.y);
	int d = min(viewPort.bottom(), topLeft.y + h);
	if (a >= b || c >= d) { return; }

	vector<dColor> tmp(b - a, color);
	rep(j, c, d)
	{
		int position = j * targetTile.width + a;
		memcpy(targetTile.colors.data() + position, tmp.data(), (b - a) * sizeof(color));
	}
}

void drawRectangle(tile& targetTile, dbuf& zBuffer, double depth,
	dVector2 topLeft, int width, int height, dRect viewPort, dColor color) {
	int a = max(viewPort.left(), topLeft.x);
	int b = min(viewPort.right(), topLeft.x + width);
	int c = max(viewPort.top(), topLeft.y);
	int d = min(viewPort.bottom(), topLeft.y + height);
	if (a >= b || c >= d) { return; }

	rep(i, a, b) rep(j, c, d) {
		int position = j * targetTile.width + i;// j 行 i 列
		if (zBuffer[position] <= depth)
		{
			zBuffer[position] = depth;
			targetTile.colors[position] = color;
		}
	}
}

void drawRectangleWithTransform(tile& targetTile, dbuf& zBuffer, double depth,
	vector2 center, double width, double height, matrix2 transformMatrix, dRect viewPort, dColor color) {
	matrix2 transformInverse = transformMatrix.inv();
	vector2 transformHalfWidthHeight = transformMatrix.abs() * vector2(width, height) / 2;
	// 这个 1 是经验公式。我没做严格论证。
	int a = max(viewPort.left(), int(center.x - transformHalfWidthHeight.x) - 1);
	int b = min(viewPort.right(), int(center.x + transformHalfWidthHeight.x) + 1);
	int c = max(viewPort.top(), int(center.y - transformHalfWidthHeight.y) - 1);
	int d = min(viewPort.bottom(), int(center.y + transformHalfWidthHeight.y) + 1);
	if (a >= b || c >= d) { return; }

	rep(i, a, b) rep(j, c, d) {
		dVector2 dPoint(i, j);
		// 下面式子的正确性从逆变换比较容易看，要注意四则运算的优先级。
		vector2 uPoint = vector2(0.5) +
			transformInverse * (vector2(dPoint) - center) / vector2(width, height);
		if (isInside(uPoint, { 1, 1 })) {
			int depthIndex = dPoint.y * targetTile.width + dPoint.x;
			if (zBuffer[depthIndex] <= depth)
			{
				zBuffer[depthIndex] = depth;
				targetTile.colors[depthIndex] = color;
			}
		}
	}
}

void drawEllipse(tile& targetTile, dbuf& zBuffer, double depth,
	vector2 center, double axisX, double axisY, dRect viewPort, dColor color) {
	int a = max(viewPort.left(), int(center.x - axisX) - 1);
	int b = min(viewPort.right(), int(center.x + axisX) + 1);
	int c = max(viewPort.top(), int(center.y - axisY) - 1);
	int d = min(viewPort.bottom(), int(center.y + axisY) + 1);
	double inv_aax = 1 / (axisX * axisX);
	double inv_aay = 1 / (axisY * axisY);

	rep(i, a, b) rep(j, c, d) {
		vector2 v = vector2(i, j) - center;
		double tmp =
			v.x * v.x * inv_aax +
			v.y * v.y * inv_aay;
		if (tmp < 1) {
			int position = j * targetTile.width + i;
			if (zBuffer[position] <= depth) {
				zBuffer[position] = depth;
				targetTile.colors[position] = color;
			}
		}
	}
}
