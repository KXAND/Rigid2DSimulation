#include "DrawTile.h"

void drawBasedTile(Tile& dest, DVector2 tl, Tile const& src)
{
	auto viewportDest = dest.Rect();
	auto viewportSrc = src.Rect();

	int l = max(viewportDest.GetLeft(), tl.x);
	int r = min(viewportDest.GetRight(), tl.x + viewportSrc.w);
	int t = max(viewportDest.GetTop(), tl.y);
	int b = min(viewportDest.GetBottom(), tl.y + viewportSrc.h);
	if (l >= r || t >= b) { return; } // 坐标系为左上为原点，所以正常情况是 l<r, t<b

	for (int j = (t), jEnd = int(b); j < jEnd; ++j)
	{
		int destP = j * dest.width + l;
		int srcP = (j - tl.y + viewportSrc.GetTop()) * src.width +
			(l - tl.x + viewportSrc.GetLeft());//将点/像素从世界坐标系转换到scrtile的内部坐标系
		int rowSize = sizeof(Color) * (r - l);
		memcpy(dest.colors.data() + destP, src.colors.data() + srcP, rowSize);
	}
}

void drawTile(Tile& dest, DepthBuffer& zBuffer, double depth, DVector2 tl,
	DRect vp_dest, Tile const& src, DRect viewportSrc)
{
	int l = max(vp_dest.GetLeft(), tl.x);
	int r = min(vp_dest.GetRight(), tl.x + viewportSrc.w);
	int t = max(vp_dest.GetTop(), tl.y);
	int b = min(vp_dest.GetBottom(), tl.y + viewportSrc.h);
	if (l >= r || t >= b) { return; }

	for (int j = (t), jEnd = int(b); j < jEnd; ++j)
	{
		int destPxl = j * dest.width + l;
		int srcPxl = (j - tl.y + viewportSrc.GetTop()) * src.width +
			(l - tl.x + viewportSrc.GetLeft());
		for (int i = (l), iEnd = int(r); i < iEnd; ++i)
		{
			if (zBuffer[destPxl] <= depth && src.as[srcPxl] != 0)
			{
				zBuffer[destPxl] = depth; dest.colors[destPxl] = src.colors[srcPxl];
			}
			destPxl++;
			srcPxl++;
		}
	}
}

// 疑似弃用
bool hitTile(DVector2 p, DVector2 tl, Tile const& tileSrc, DRect viewportSrc)
{
	// 假设了vpsrc和 tilesrc 左上角对齐
	DVector2 pSrc = viewportSrc.topLeftPosition + p - tl;
	int pxl = pSrc.y * tileSrc.width + pSrc.x;
	return IsInside(pSrc, viewportSrc) && tileSrc.as[pxl] != 0;
}


