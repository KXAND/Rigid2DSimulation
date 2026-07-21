#pragma once
#include "Vector2.h"
#include "Tile.h"

// @todo: 把两个 DrawRectangle 移到别的文件中
void DrawRectangleRaw(Tile& targetTile, DVector2 topLeft, int width, int height, DRect viewPort, Color color);

void DrawRectangle(Tile& targetTile, DepthBuffer& zBuffer, double depth,
	DVector2 topLeft, int width, int height, DRect viewPort, Color color);

void DrawRectangleWithTransform(Tile& targetTile, DepthBuffer& zBuffer, double depth,
	Vector2 center, double width, double height, Matrix2 transformMatrix, DRect viewPort, Color color);

void DrawEllipse(Tile& targetTile, DepthBuffer& zBuffer, double depth,
	Vector2 center, double axisX, double axisY, DRect viewPort, Color color);

