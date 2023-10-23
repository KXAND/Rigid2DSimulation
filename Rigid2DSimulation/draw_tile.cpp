#include "draw_tile.h"

void draw_tile_raw
(tile& dest, dVector2 tl, dRect vp_dest, tile const& src, dRect vp_src) {
	int a = max(vp_dest.left(), tl.x);
	int b = min(vp_dest.right(), tl.x + vp_src.w);
	int c = max(vp_dest.top(), tl.y);
	int d = min(vp_dest.bottom(), tl.y + vp_src.h);
	if (a >= b || c >= d) { return; }

	rep(j, c, d) {
		int dp = j * dest.width + a;
		int sp = (j - tl.y + vp_src.top()) * src.width +
			(a - tl.x + vp_src.left());
		int sz_row = sizeof(dColor) * (b - a);
		memcpy(dest.colors.data() + dp, src.colors.data() + sp, sz_row);
	}
}
void draw_tile(tile& dest, dbuf& ds, double dep, dVector2 tl,
	dRect vp_dest, tile const& src, dRect vp_src) {
	int a = max(vp_dest.left(), tl.x);
	int b = min(vp_dest.right(), tl.x + vp_src.w);
	int c = max(vp_dest.top(), tl.y);
	int d = min(vp_dest.bottom(), tl.y + vp_src.h);
	if (a >= b || c >= d) { return; }

	rep(j, c, d) {
		int dp = j * dest.width + a;
		int sp = (j - tl.y + vp_src.top()) * src.width +
			(a - tl.x + vp_src.left());
		rep(i, a, b) {
			if (ds[dp] <= dep && src.as[sp] != 0) {
				ds[dp] = dep; dest.colors[dp] = src.colors[sp];
			} dp++; sp++;
		}
	}
}
bool hit_tile(dVector2 p, dVector2 tl, tile const& src, dRect vp_src) {
	dVector2 s_pnt = vp_src.topLeftPosition + p - tl;
	int sp = s_pnt.y * src.width + s_pnt.x;
	return isInside(s_pnt, vp_src) && src.as[sp] != 0;
}
