#pragma once
#include "Tile.h"

void subDrawTriangle(tile& dest, dbuf& ds,
	DVector2 pa, DVector2 pb, DVector2 pc, double d, dColor col, bool same_y);

void drawTriangle(tile& targetTile, dbuf& zBuffer, DRect const& viewPort,
	DVector2 pa, DVector2 pb, DVector2 pc, double depth, dColor color);

void drawTriangle(tile& targetTile, dbuf& zBuffer, DRect const& viewPort,
	Vector2 pa, Vector2 pb, Vector2 pc, double depth, dColor color);

