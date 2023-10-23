#pragma once
#include "tile.h"

void subDrawTriangle(tile& dest, dbuf& ds,
	dVector2 pa, dVector2 pb, dVector2 pc, double d, dColor col, bool same_y);
void draw_tri(tile& dest, dbuf& ds, dRect const& vp,
	dVector2 pa, dVector2 pb, dVector2 pc, double d, dColor col);
void draw_tri(tile& dest, dbuf& ds, dRect const& vp,
	vector2 pa, vector2 pb, vector2 pc, double d, dColor col);
