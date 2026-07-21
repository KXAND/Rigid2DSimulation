#pragma once
#include "Vector2.h"
#include "Tile.h"

void drawRectangleWithBorderRaw(tile& targetTile, DVector2 topLeft, int width,
	int height, DRect viewPort, dColor color, dColor borderColor);


void drawRectangleWithBorder(tile& targetTile, dbuf& zBuffer, double depth,
	DVector2 topLeft, int width, int height, DRect viewPort, dColor color, dColor borderColor);

