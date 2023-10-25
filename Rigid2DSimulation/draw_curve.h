#pragma once
#include "vector2.h"
#include "tile.h"

void drawEllipseBorder(tile& targetTile, dbuf& zBuffer, double depth,
	vector2 center, double axisX, double axisY, dRect viewPort, dColor color, int n);
