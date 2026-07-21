#include "draw_comp.h"
#include "draw_geo.h"

void drawRectangleWithBorderRaw(tile& targetTile, 
	DVector2 topLeft, int width, int height, DRect viewPort, dColor color, dColor borderColor)
{
	// 左上为原点，向下向左

	// 内部
	drawRectangleRaw(targetTile, topLeft, width, height, viewPort, color);

	DVector2 topRight = topLeft + DVector2(width, 0);
	DVector2 bottomLeft = topLeft + DVector2(0, height);

	// 框颜色
	drawRectangleRaw(targetTile, topLeft, width, 1, viewPort, borderColor);
	drawRectangleRaw(targetTile, bottomLeft, width, 1, viewPort, borderColor);
	drawRectangleRaw(targetTile, topLeft, 1, height, viewPort, borderColor);
	drawRectangleRaw(targetTile, topRight, 1, height, viewPort, borderColor);
}

// 含深度的矩形
void drawRectangleWithBorder(tile& targetTile, dbuf& zBuffer, double depth,
	DVector2 topLeft, int width, int height, DRect viewPort, dColor color, dColor borderColor)
{
	// 矩形内部绘制
	drawRectangle(targetTile, zBuffer, depth, topLeft, width, height, viewPort, color);


	// 绘制矩形外框
	DVector2 topRight = topLeft + DVector2(width, 0);
	DVector2 bottomLeft = topLeft + DVector2(0, height);

	drawRectangle(targetTile, zBuffer, depth, topLeft, width, 1, viewPort, color);
	drawRectangle(targetTile, zBuffer, depth, bottomLeft, width, 1, viewPort, color);
	drawRectangle(targetTile, zBuffer, depth, topLeft, 1, height, viewPort, color);
	drawRectangle(targetTile, zBuffer, depth, topRight, 1, height, viewPort, color);
}

