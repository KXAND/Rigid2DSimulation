#pragma once
#include "vector2.h"
#include "tile.h"

void drawRectangleRaw(tile& dest, dVector2 tl, int w, int h, dRect vp, dColor col);
void drawRectangle(tile& dest, dbuf& ds, double dep,
	dVector2 tl, int w, int h, dRect vp, dColor col);
void drawRectangleWithTransform(tile& dest, dbuf& ds, double dep,
	vector2 ct, double w, double h, matrix2 tsf, dRect vp, dColor col);
void drawEllipse(tile& dest, dbuf& ds, double dep,
	vector2 ct, double ax, double ay, dRect vp, dColor col);
