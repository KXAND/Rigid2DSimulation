#include "draw_tile.h"

void drawBasedTile(tile& dest, dVector2 tl, tile const& src)
{
	auto viewportDest = dest.rect();
	auto viewportSrc = src.rect();

	int l = max(viewportDest.left(), tl.x);
	int r = min(viewportDest.right(), tl.x + viewportSrc.w);
	int t = max(viewportDest.top(), tl.y);
	int b = min(viewportDest.bottom(), tl.y + viewportSrc.h);
	if (l >= r || t >= b) { return; } // ����ϵΪ����Ϊԭ�㣬������������� l<r, t<b

	rep(j, t, b)
	{
		int destP = j * dest.width + l;
		int srcP = (j - tl.y + viewportSrc.top()) * src.width +
			(l - tl.x + viewportSrc.left());//����/���ش���������ϵת����scrtile���ڲ�����ϵ
		int rowSize = sizeof(dColor) * (r - l);
		memcpy(dest.colors.data() + destP, src.colors.data() + srcP, rowSize);
	}
}

void drawTile(tile& dest, dbuf& zBuffer, double dep, dVector2 tl,
	dRect vp_dest, tile const& src, dRect viewportSrc)
{
	int l = max(vp_dest.left(), tl.x);
	int r = min(vp_dest.right(), tl.x + viewportSrc.w);
	int t = max(vp_dest.top(), tl.y);
	int b = min(vp_dest.bottom(), tl.y + viewportSrc.h);
	if (l >= r || t >= b) { return; }

	rep(j, t, b)
	{
		int destPxl = j * dest.width + l;
		int srcPxl = (j - tl.y + viewportSrc.top()) * src.width +
			(l - tl.x + viewportSrc.left());
		rep(i, l, r)
		{
			if (zBuffer[destPxl] <= dep && src.as[srcPxl] != 0)
			{
				zBuffer[destPxl] = dep; dest.colors[destPxl] = src.colors[srcPxl];
			}
			destPxl++;
			srcPxl++;
		}
	}
}

// ��������
bool hitTile(dVector2 p, dVector2 tl, tile const& tileSrc, dRect viewportSrc)
{
	// ������vpsrc�� tilesrc ���ϽǶ���
	dVector2 pSrc = viewportSrc.topLeftPosition + p - tl;
	int pxl = pSrc.y * tileSrc.width + pSrc.x;
	return isInside(pSrc, viewportSrc) && tileSrc.as[pxl] != 0;
}
