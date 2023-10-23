#pragma once
#include "vector3.h"
#include "tile.h"

bool preDrawLineSegment(dVector2& pa, dVector2& pb, dRect vp);
void drawLineSegment(tile& dest, dbuf& ds, 
	dVector2 pa, dVector2 pb, double d, dRect vp, dColor c);
void drawLineSegment(tile& dest, dbuf& ds, 
	vector2 pa, vector2 pb, double d, dRect vp, dColor c);
