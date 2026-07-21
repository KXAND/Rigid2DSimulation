#include "DrawBorderedRectangle.h"
#include "DrawGeometry.h"

void DrawRectangleWithBorderRaw(Tile& targetTile,
	DVector2 topLeft, int width, int height, DRect viewPort, Color color, Color borderColor)
{
	// 左上为原点，向下向左

	// 内部
	DrawRectangleRaw(targetTile, topLeft, width, height, viewPort, color);

	DVector2 topRight = topLeft + DVector2(width, 0);
	DVector2 bottomLeft = topLeft + DVector2(0, height);

	// 框颜色
	DrawRectangleRaw(targetTile, topLeft, width, 1, viewPort, borderColor);
	DrawRectangleRaw(targetTile, bottomLeft, width, 1, viewPort, borderColor);
	DrawRectangleRaw(targetTile, topLeft, 1, height, viewPort, borderColor);
	DrawRectangleRaw(targetTile, topRight, 1, height, viewPort, borderColor);
}

// 含深度的矩形
void DrawRectangleWithBorder(Tile& targetTile, DepthBuffer& zBuffer, double depth,
	DVector2 topLeft, int width, int height, DRect viewPort, Color color, Color borderColor)
{
	// 矩形内部绘制
	DrawRectangle(targetTile, zBuffer, depth, topLeft, width, height, viewPort, color);


	// 绘制矩形外框
	DVector2 topRight = topLeft + DVector2(width, 0);
	DVector2 bottomLeft = topLeft + DVector2(0, height);

	DrawRectangle(targetTile, zBuffer, depth, topLeft, width, 1, viewPort, color);
	DrawRectangle(targetTile, zBuffer, depth, bottomLeft, width, 1, viewPort, color);
	DrawRectangle(targetTile, zBuffer, depth, topLeft, 1, height, viewPort, color);
	DrawRectangle(targetTile, zBuffer, depth, topRight, 1, height, viewPort, color);
}

