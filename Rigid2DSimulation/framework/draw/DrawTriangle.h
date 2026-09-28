#pragma once
#include "Tile.h"

void DrawTriangleHalf(Tile& targetTile, DepthBuffer& zBuffer,
                      DVector2 firstPoint, DVector2 middlePoint, DVector2 lastPoint, double depth, Color color, bool isYAxisMajor);

void DrawTriangle(Tile& targetTile, DepthBuffer& zBuffer, DRect const& viewport,
                  DVector2 firstPoint, DVector2 middlePoint, DVector2 lastPoint, double depth, Color color);

void DrawTriangle(Tile& targetTile, DepthBuffer& zBuffer, DRect const& viewport,
                  Vector2 firstPoint, Vector2 middlePoint, Vector2 lastPoint, double depth, Color color);
