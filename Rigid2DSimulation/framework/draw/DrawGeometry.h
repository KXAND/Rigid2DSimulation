#pragma once
#include "Vector2.h"
#include "Tile.h"

// @todo: 把两个 drawRectangle 移到别的文件中
void drawRectangleRaw(Tile& targetTile, DVector2 topLeft, int w, int h, DRect viewPort, Color color);

void drawRectangle(Tile& targetTile, DepthBuffer& zBuffer, double depth,
	DVector2 topLeft, int width, int height, DRect viewPort, Color color);

void drawRectangleWithTransform(Tile& targetTile, DepthBuffer& zBuffer, double depth,
	Vector2 center, double width, double height, Matrix2 transformMatrix, DRect viewPort, Color color);

void drawEllipse(Tile& targetTile, DepthBuffer& zBuffer, double depth,
	Vector2 center, double axisX, double axisY, DRect viewPort, Color color);

