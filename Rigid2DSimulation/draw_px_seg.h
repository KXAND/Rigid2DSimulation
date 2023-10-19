#pragma once
#include "vector3.h"
#include "tile.h"

bool pre_draw_px_seg(dvec& pa, dvec& pb, drect vp);
void draw_px_seg(tile& dest, dbuf& ds, 
	dvec pa, dvec pb, double d, drect vp, dcol c);
void draw_px_seg(tile& dest, dbuf& ds, 
	vector2 pa, vector2 pb, double d, drect vp, dcol c);
