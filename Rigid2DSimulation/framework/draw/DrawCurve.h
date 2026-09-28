#pragma once
#include "Vector2.h"
#include "Tile.h"

void DrawEllipseBorder(Tile& targetTile, DepthBuffer& zBuffer, double depth,
                       Vector2 center, double horizontalRadius, double verticalRadius, DRect viewport, Color color, int segmentCount);
