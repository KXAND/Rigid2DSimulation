#pragma once
#include "vector2.h"
#include "tile.h"

void drawRectangleWithBorderRaw(tile& targetTile, DVector2 topLeft, int width,
	int height, DRect viewPort, dColor color, dColor borderColor);


void drawRectangleWithBorder(tile& targetTile, dbuf& zBuffer, double depth,
	DVector2 topLeft, int width, int height, DRect viewPort, dColor color, dColor borderColor);

