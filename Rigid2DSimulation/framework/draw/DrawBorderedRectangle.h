#pragma once
#include "Vector2.h"
#include "Tile.h"

void drawRectangleWithBorderRaw(Tile& targetTile, DVector2 topLeft, int width,
	int height, DRect viewPort, Color color, Color borderColor);


void drawRectangleWithBorder(Tile& targetTile, DepthBuffer& zBuffer, double depth,
	DVector2 topLeft, int width, int height, DRect viewPort, Color color, Color borderColor);

