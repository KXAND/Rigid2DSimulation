#pragma once
#include "tile.h"

void subDrawTriangle(tile& dest, dbuf& ds,
	dVector2 pa, dVector2 pb, dVector2 pc, double d, dColor col, bool same_y);

void drawTriangle(tile& targetTile, dbuf& zBuffer, dRect const& viewPort,
	dVector2 pa, dVector2 pb, dVector2 pc, double depth, dColor color);

void drawTriangle(tile& targetTile, dbuf& zBuffer, dRect const& viewPort,
	vector2 pa, vector2 pb, vector2 pc, double depth, dColor color);
