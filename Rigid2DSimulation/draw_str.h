#pragma once
#include "font.h"

void draw_char(tile& dest, dbuf& ds, double dep,
	dVector2 tl, dRect vp_dest, tile const& src, dColor col);
// w 为 0 时代表不换行。
void draw_str(tile& dest, dbuf& ds, double d, wstring const& s,
	dColor col, font const& ft, dVector2& tl_cur, int& x_cur, int w, dRect vp);
void draw_str(tile& dest, dbuf& ds, double d, wstring const& s,
	dColor col, font const& ft, dVector2 tl, int w, dRect vp);

dVector2 str_wh(wstring const& s, font const& ft, int w, int* x_last = NULL);
// 如果不想考虑高的话，就设置 p.y = 0。
int str_index_at_pos(wstring const& s, font const& ft, dVector2 p, int w);
