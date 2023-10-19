#pragma once
#include "vector2.h"
#include "tile.h"

void draw_eclipse_frame(tile& dest, dbuf& ds, double dep,
	vector2 ct, double ax, double ay, drect vp, dcol col, int n);
