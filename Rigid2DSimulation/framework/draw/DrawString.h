#pragma once
#include "Font.h"

void DrawCharacter(Tile& dest, DepthBuffer& ds, double depth,
	DVector2 tl, DRect vp_dest, Tile const& src, Color col);
// w 为 0 时代表不换行。
void DrawString(Tile& dest, DepthBuffer& ds, double d, wstring const& s,
	Color col, Font const& ft, DVector2& tl_cur, int& x_cur, int w, DRect vp);
void DrawString(Tile& dest, DepthBuffer& ds, double d, wstring const& s,
	Color col, Font const& ft, DVector2 tl, int w, DRect vp);

DVector2 GetStringSize(wstring const& s, Font const& ft, int w, int* x_last = NULL);
// 如果不想考虑高的话，就设置 p.y = 0。
int GetStringIndexAtPosition(wstring const& s, Font const& ft, DVector2 p, int w);


