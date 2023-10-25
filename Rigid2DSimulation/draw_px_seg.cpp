#include "draw_px_seg.h"

// 好像这里经常发生溢出问题，不知能不能处理一下。
// 把 int 换成 long long 了，应该会有帮助。
bool preDrawLineSegment(dVector2& pa, dVector2& pb, dRect viewPort) {
	long long x0 = viewPort.left();
	long long x1 = viewPort.right() - 1;
	long long y0 = viewPort.top();
	long long y1 = viewPort.bottom() - 1;

	// 线性插值
	// 下面假设了不会发生除以 0 的问题。
#define TMP(a, b, x, y, bd, symbol)\
if (p##a.x symbol bd) {\
	p##a.y +=  (p##b.y - p##a.y) * (bd - p##a.x) / (p##b.x - p##a.x);\
	p##a.x = bd;\
}

	bool b0 = pa.x < x0 && pb.x < x0;
	bool b1 = pa.x > x1 && pb.x > x1;
	if (b0 || b1) { return false; }
	TMP(a, b, x, y, x0, < );
	TMP(a, b, x, y, x1, > );
	TMP(b, a, x, y, x0, < );
	TMP(b, a, x, y, x1, > );

	b0 = pa.y < y0 && pb.y < y0;
	b1 = pa.y > y1 && pb.y > y1;
	if (b0 || b1) { return false; }
	TMP(a, b, y, x, y0, < );
	TMP(a, b, y, x, y1, > );
	TMP(b, a, y, x, y0, < );
	TMP(b, a, y, x, y1, > ); return true;
#undef TMP
}
void drawLineSegment(tile& targetTile, dbuf& zBuffer,
	dVector2 pa, dVector2 pb, double d, dRect viewPort, dColor color) 
{
	if (viewPort.w <= 0 || viewPort.h <= 0) { return; }
	if (!preDrawLineSegment(pa, pb, viewPort)) { return; }

	bool changed = false;
	int x = pa.x; int y = pa.y;
	int dx = abs(pb.x - pa.x);
	int dy = abs(pb.y - pa.y);
	int sx = (pb.x > pa.x) ? 1 : -1;
	int sy = (pb.y > pa.y) ? 1 : -1;
	if (dy > dx) { swap(dx, dy); changed = true; }

	int e = dx;
	int id = y * targetTile.width + x;
	if (zBuffer[id] <= d) { zBuffer[id] = d; targetTile.colors[id] = color; }

	rep(i, 0, dx) {
		e -= 2 * dy;
		if (changed) { y += sy; }
		else { x += sx; }
		if (e < 0) {
			e += 2 * dx;
			if (changed) { x += sx; }
			else { y += sy; }
		}
		int id = y * targetTile.width + x;
		if (zBuffer[id] <= d) { zBuffer[id] = d; targetTile.colors[id] = color; }
	}
}
void drawLineSegment(tile& targetTile, dbuf& zBuffer,
	vector2 pa, vector2 pb, double depth, dRect viewPort, dColor color)
{
	drawLineSegment(targetTile, zBuffer, (dVector2)pa, (dVector2)pb, depth, viewPort, color);
}
