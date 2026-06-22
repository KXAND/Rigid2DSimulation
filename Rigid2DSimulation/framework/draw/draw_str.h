#pragma once
#include "font.h"

void draw_char(tile& dest, dbuf& ds, double dep,
	dVector2 tl, dRect vp_dest, tile const& src, dColor col);
// w Ϊ 0 ʱ���������С�
void draw_str(tile& dest, dbuf& ds, double d, wstring const& s,
	dColor col, font const& ft, dVector2& tl_cur, int& x_cur, int w, dRect vp);
void draw_str(tile& dest, dbuf& ds, double d, wstring const& s,
	dColor col, font const& ft, dVector2 tl, int w, dRect vp);

dVector2 str_wh(wstring const& s, font const& ft, int w, int* x_last = NULL);
// ������뿼�ǸߵĻ��������� p.y = 0��
int str_index_at_pos(wstring const& s, font const& ft, dVector2 p, int w);

