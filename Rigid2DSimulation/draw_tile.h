#pragma once
#include "vector2.h"
#include "tile.h"

// ���� vp_dest �������죬���� vp_src ���������졣
void draw_tile_raw(tile& dest, dVector2 tl, dRect vp_dest, tile const& src, dRect vp_src);
void draw_tile(tile& dest, dbuf& ds, double dep, dVector2 tl, dRect vp_dest, tile const& src, dRect vp_src);
bool hit_tile(dVector2 p, dVector2 tl, tile const& src, dRect vp_src);
