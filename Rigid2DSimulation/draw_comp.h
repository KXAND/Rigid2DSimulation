#pragma once
#include "vector2.h"
#include "tile.h"

void drawRectangleWithBorderRaw
(tile& dest, dVector2 tl, int w, int h, dRect vp, dColor c, dColor fc);
void drawRectangleBorder(tile& dest, dbuf& ds, double d,
	dVector2 tl, int w, int h, dRect vp, dColor c);
void drawRectangleWithBorder(tile& dest, dbuf& ds, double d,
	dVector2 tl, int w, int h, dRect vp, dColor c, dColor fc);
