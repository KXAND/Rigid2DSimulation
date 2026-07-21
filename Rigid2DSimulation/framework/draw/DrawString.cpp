#include "DrawString.h"
#include "Font.h"

void DrawCharacter(Tile& dest, DepthBuffer& ds, double depth,
	DVector2 tl, DRect vp_dest, Tile const& src, Color col) {
	int a = max(vp_dest.GetLeft(), tl.x);
	int b = min(vp_dest.GetRight(), tl.x + src.width);
	int c = max(vp_dest.GetTop(), tl.y);
	int d = min(vp_dest.GetBottom(), tl.y + src.height);
	if (a >= b || c >= d) { return; }

	for (int j = (c), jEnd = int(d); j < jEnd; ++j) {
		int dp = j * dest.width + a;
		int sp = (j - tl.y) * src.width + a - tl.x;
		for (int i = (a), iEnd = int(b); i < iEnd; ++i) {
			if (ds[dp] <= depth && src.as[sp] != 0) {
				ds[dp] = depth; dest.colors[dp] = col;
			} dp++; sp++;
		}
	}
}
void DrawString(Tile& dest, DepthBuffer& ds, double d, wstring const& s,
	Color col, Font const& ft, DVector2& tl_cur, int& x_cur, int w, DRect vp) {
	for (auto c : s) {
		if (c == '\n') { tl_cur.y += ft.h; x_cur = 0; }
		else {
			Tile const& t = ft[c];
			if (w != 0 && x_cur > 0 && x_cur + t.width > w) {
				tl_cur.y += ft.h; x_cur = 0;
			} 
			DVector2 tmp = tl_cur + DVector2(x_cur, 0);
			DrawCharacter(dest, ds, d, tmp, vp, t, col); x_cur += t.width;
		}
	}
}
void DrawString(Tile& dest, DepthBuffer& ds, double d, wstring const& s,
	Color c, Font const& ft, DVector2 tl, int w, DRect vp) {
	int x_cur = 0;
	DrawString(dest, ds, d, s, c, ft, tl, x_cur, w, vp);
}

DVector2 GetStringSize(wstring const& s, Font const& ft, int w, int* x_last) {
	int x_cur = 0, w_out = 0, h_out = ft.h;
	for (auto c : s) {
		if (c == L'\n') {
			w_out = max(w_out, x_cur);
			h_out += ft.h; x_cur = 0;
		} else {
			Tile const& t = ft[c];
			if (w != 0 && x_cur > 0 && x_cur + t.width > w) {
				w_out = max(w_out, x_cur);
				h_out += ft.h; x_cur = 0;
			}
			// 下面两个顺序可别乱动啊。
			x_cur += t.width; w_out = max(w_out, x_cur);
		}
	} 
	if (x_last) { *x_last = x_cur; } 
	return { w_out, h_out };
}
int GetStringIndexAtPosition(wstring const& s, Font const& ft, DVector2 p, int w) {
	int x_cur = 0, y_cur = 0;
	if (0 > p.y) { return 0; }

	for (int ind = (0), indEnd = int(s.size()); ind < indEnd; ++ind) {
		wchar_t c = s[ind];
		if (c == L'\n') {
			y_cur += ft.h; x_cur = 0;
			if (y_cur > p.y) { return ind; }
		} else {
			Tile const& t = ft[c];
			if (w != 0 && x_cur > 0 && x_cur + t.width > w) {
				y_cur += ft.h; x_cur = 0;
				if (y_cur > p.y) { return ind; }
			} x_cur += t.width;
			if (x_cur > p.x && y_cur + ft.h > p.y) {
				return x_cur - p.x > t.width / 2 ? ind : ind + 1;
			}
		}
	} return s.size();
}


