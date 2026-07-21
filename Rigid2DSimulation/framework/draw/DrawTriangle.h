#pragma once
#include "Tile.h"

void subDrawTriangle(Tile& dest, DepthBuffer& ds,
	DVector2 pa, DVector2 pb, DVector2 pc, double d, Color col, bool same_y);

void drawTriangle(Tile& targetTile, DepthBuffer& zBuffer, DRect const& viewPort,
	DVector2 pa, DVector2 pb, DVector2 pc, double depth, Color color);

void drawTriangle(Tile& targetTile, DepthBuffer& zBuffer, DRect const& viewPort,
	Vector2 pa, Vector2 pb, Vector2 pc, double depth, Color color);

