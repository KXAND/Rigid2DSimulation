#pragma once
#include "Vector2.h"
#include "Tile.h"

void drawEllipseBorder(Tile& targetTile, DepthBuffer& zBuffer, double depth,
	Vector2 center, double axisX, double axisY, DRect viewPort, Color color, int n);

