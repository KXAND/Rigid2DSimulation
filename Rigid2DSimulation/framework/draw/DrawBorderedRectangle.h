#pragma once
#include "Vector2.h"
#include "Tile.h"

void DrawRectangleWithBorderRaw(Tile& targetTile, DVector2 topLeft, int width,
                                int height, DRect viewport, Color color, Color borderColor);

void DrawRectangleWithBorder(Tile& targetTile, DepthBuffer& zBuffer, double depth,
                             DVector2 topLeft, int width, int height, DRect viewport, Color color, Color borderColor);
