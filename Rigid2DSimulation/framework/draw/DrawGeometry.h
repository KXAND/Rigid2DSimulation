#pragma once
#include "Vector2.h"
#include "Tile.h"

// @todo: 把两个 drawRectangle 移到别的文件中
void drawRectangleRaw(tile& targetTile, DVector2 topLeft, int w, int h, DRect viewPort, dColor color);

void drawRectangle(tile& targetTile, dbuf& zBuffer, double depth,
	DVector2 topLeft, int width, int height, DRect viewPort, dColor color);

void drawRectangleWithTransform(tile& targetTile, dbuf& zBuffer, double depth,
	Vector2 center, double width, double height, Matrix2 transformMatrix, DRect viewPort, dColor color);

void drawEllipse(tile& targetTile, dbuf& zBuffer, double depth,
	Vector2 center, double axisX, double axisY, DRect viewPort, dColor color);

