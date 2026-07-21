#pragma once
#include "Tile.h"

void DrawTriangleHalf(Tile& targetTile, DepthBuffer& zBuffer,
	DVector2 firstPoint, DVector2 middlePoint, DVector2 lastPoint, double d, Color color, bool isYAxisMajor);

void DrawTriangle(Tile& targetTile, DepthBuffer& zBuffer, DRect const& viewPort,
	DVector2 firstPoint, DVector2 middlePoint, DVector2 lastPoint, double depth, Color color);

void DrawTriangle(Tile& targetTile, DepthBuffer& zBuffer, DRect const& viewPort,
	Vector2 firstPoint, Vector2 middlePoint, Vector2 lastPoint, double depth, Color color);

