#pragma once
#include "Vector2.h"
#include "Tile.h"

void drawEllipseBorder(tile& targetTile, dbuf& zBuffer, double depth,
	Vector2 center, double axisX, double axisY, DRect viewPort, dColor color, int n);

