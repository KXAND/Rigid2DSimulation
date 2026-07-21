#pragma once
#include "font.h"

void draw_char(tile& dest, dbuf& ds, double depth,
	DVector2 tl, DRect vp_dest, tile const& src, dColor col);
// w 为 0 时代表不换行。
void draw_str(tile& dest, dbuf& ds, double d, wstring const& s,
	dColor col, font const& ft, DVector2& tl_cur, int& x_cur, int w, DRect vp);
void draw_str(tile& dest, dbuf& ds, double d, wstring const& s,
	dColor col, font const& ft, DVector2 tl, int w, DRect vp);

DVector2 str_wh(wstring const& s, font const& ft, int w, int* x_last = NULL);
// 如果不想考虑高的话，就设置 p.y = 0。
int str_index_at_pos(wstring const& s, font const& ft, DVector2 p, int w);


