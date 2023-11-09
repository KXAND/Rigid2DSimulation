#include "draw_tile.h"

void drawTileRaw(tile& dest, dVector2 tl, tile const& src)
{
	auto viewportDest = dest.rect();
	auto viewportSrc = dest.rect();

	int l = max(viewportDest.left(), tl.x);
	int r = min(viewportDest.right(), tl.x + viewportSrc.w);
	int t = max(viewportDest.top(), tl.y);
	int b = min(viewportDest.bottom(), tl.y + viewportSrc.h);
	if (l >= r || t >= b) { return; } // 坐标系为左上为原点，所以正常情况是 l<r, t<b

	rep(j, t, b)
	{
		int destP = j * dest.width + l;
		int srcP = (j - tl.y + viewportSrc.top()) * src.width +
			(l - tl.x + viewportSrc.left());//算式变复杂是因为需要从世界坐标系转换到scrtile的内部坐标系
		int rowSize = sizeof(dColor) * (r - l);
		memcpy(dest.colors.data() + destP, src.colors.data() + srcP, rowSize);
	}
}

void draw_tile(tile& dest, dbuf& ds, double dep, dVector2 tl,
	dRect vp_dest, tile const& src, dRect vp_src)
{
	int a = max(vp_dest.left(), tl.x);
	int b = min(vp_dest.right(), tl.x + vp_src.w);
	int c = max(vp_dest.top(), tl.y);
	int d = min(vp_dest.bottom(), tl.y + vp_src.h);
	if (a >= b || c >= d) { return; }

	rep(j, c, d)
	{
		int dp = j * dest.width + a;
		int sp = (j - tl.y + vp_src.top()) * src.width +
			(a - tl.x + vp_src.left());
		rep(i, a, b)
		{
			if (ds[dp] <= dep && src.as[sp] != 0)
			{
				ds[dp] = dep; dest.colors[dp] = src.colors[sp];
			} dp++; sp++;
		}
	}
}
bool hit_tile(dVector2 p, dVector2 tl, tile const& src, dRect vp_src)
{
	dVector2 s_pnt = vp_src.topLeftPosition + p - tl;
	int sp = s_pnt.y * src.width + s_pnt.x;
	return isInside(s_pnt, vp_src) && src.as[sp] != 0;
}
