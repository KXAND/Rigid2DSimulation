#pragma once
#include "vector2.h"
#include "tile.h"

void drawRectangleWithBorderRaw(tile& targetTile, dVector2 topLeft, int width,
	int height, dRect viewPort, dColor color, dColor borderColor);


void drawRectangleWithBorder(tile& targetTile, dbuf& zBuffer, double depth,
	dVector2 topLeft, int width, int height, dRect viewPort, dColor color, dColor borderColor);
