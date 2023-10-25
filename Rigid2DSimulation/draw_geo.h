#pragma once
#include "vector2.h"
#include "tile.h"

// @todo: 把两个 drawRectangle 移到别的文件中
void drawRectangleRaw(tile& targetTile, dVector2 topLeft, int w, int h, dRect viewPort, dColor color);

void drawRectangle(tile& targetTile, dbuf& zBuffer, double depth,
	dVector2 topLeft, int width, int height, dRect viewPort, dColor color);

void drawRectangleWithTransform(tile& targetTile, dbuf& zBuffer, double depth,
	vector2 center, double width, double height, matrix2 transformMatrix, dRect viewPort, dColor color);

void drawEllipse(tile& targetTile, dbuf& zBuffer, double depth,
	vector2 center, double axisX, double axisY, dRect viewPort, dColor color);
