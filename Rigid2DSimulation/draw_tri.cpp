#include "draw_tri.h"

void subDrawTriangle(tile& dest, dbuf& zBuffer,
	dVector2 pa, dVector2 pb, dVector2 pc, double depth, dColor color, bool same_y)
{
	// 假设 pa, pb, pc  已经交换过了。
	// 用它画斜的长方形有的时候会出现裂痕，不知道能不能修复。
	if (pa.y == pb.y)  return;
	int dy = pb.y > pa.y ? 1 : -1;
	int dx = pc.x > pb.x ? 1 : -1;
	for (int y = pa.y; y != pb.y + dy; y += dy)
	{
		int xb = pa.x * (pb.y - y) + pb.x * (y - pa.y); xb /= (pb.y - pa.y);
		int xc = pa.x * (pb.y - y) + pc.x * (y - pa.y); xc /= (pb.y - pa.y);
		for (int x = xb; x != xc + dx; x += dx)
		{
			int dp = same_y ? y * dest.width + x : x * dest.width + y;
			if (zBuffer[dp] <= depth) { zBuffer[dp] = depth; dest.colors[dp] = color; }
		}
	}
}
void drawTriangle(tile& targetTile, dbuf& zBuffer, dRect const& viewPort,
	dVector2 pa, dVector2 pb, dVector2 pc, double depth, dColor color)
{
	// 如果可以让这里写得好看有条理，容易修改且不容易出 bug 就好了。
	// @todo:考虑笔记中的 SH 算法

#define EdgeCut(a, c, x, y, sgn, borderLine)\
if (p##c.y sgn borderLine) { return; }/*最小值超出边界*/\
if (p##a.y sgn borderLine)/*最大值超出边界，切割*/ \
{\
	/*ac边被切割*/\
	dVector2 pac = p##c;\
	pac.y = borderLine;\
	pac.x += (p##a.x - p##c.x) * (borderLine - p##c.y) / (p##a.y - p##c.y);\
	if (pb.y sgn borderLine)\
	{\
		/*bc边也被切割，得到一个由c、pac、pbc组成的三角形*/					\
		dVector2 p1 = p##c;\
		p1.y = borderLine;\
		p1.x += (pb.x - p##c.x) * (borderLine - p##c.y) / (p##b.y - p##c.y);\
		drawTriangle(targetTile, zBuffer, viewPort, pac, p1, p##c, depth, color);\
		return;/*在递归的下一级会对其他部分进行切割，所以到此结束，下同*/\
	}\
	else {\
		/*bc边不被切割，得到一个由c、pac、pab、b组成的四边形*/\
		dVector2 p1 = pb;\
		p1.y = borderLine;\
		p1.x += (p##a.x - pb.x) * (borderLine - pb.y) / (p##a.y - pb.y);\
		drawTriangle(targetTile, zBuffer, viewPort, pac, pb, p##c, depth, color);\
		drawTriangle(targetTile, zBuffer, viewPort, pac, pb, p1, depth, color);\
		return;\
	}\
}

	// 保证x轴上左到右是　cba　的顺序
	// 这里有点低效，重复比较了，但是总的影响不大。
	if (pa.x < pb.x) swap(pa, pb);
	if (pa.x < pc.x) swap(pa, pc);
	if (pb.x < pc.x) swap(pb, pc);

	if (pa.x == pc.x) { return; } // 三角形三点不可能共线
	int dx = pa.x - pc.x;

	// 用屏幕边切割图形
	EdgeCut(a, c, y, x, > , (long long) viewPort.right() - 1);
	EdgeCut(c, a, y, x, < , (long long) viewPort.left());

	if (pa.y < pb.y) { swap(pa, pb); }
	if (pa.y < pc.y) { swap(pa, pc); }
	if (pb.y < pc.y) { swap(pb, pc); }
	if (pa.y == pc.y) { return; }
	int dy = pa.y - pc.y;
	EdgeCut(a, c, x, y, > , (long long) viewPort.bottom() - 1);
	EdgeCut(c, a, x, y, < , (long long) viewPort.top());

	// 这里是出于数值稳定性的考虑交换一下方向，否则如果三角形很扁的话会奇怪。
	bool y_big = dy > dx;
	if (!y_big)
	{
		if (pa.x < pb.x) { swap(pa, pb); }
		if (pa.x < pc.x) { swap(pa, pc); }
		if (pb.x < pc.x) { swap(pb, pc); }
		swap(pa.x, pa.y); swap(pb.x, pb.y); swap(pc.x, pc.y);
	}
	dVector2 pd = pb;
	pd.x = pa.x * (pb.y - pc.y) + pc.x * (pa.y - pb.y); pd.x /= pa.y - pc.y;
	// @ques：不是很理解这里问什么要做两遍
	subDrawTriangle(targetTile, zBuffer, pa, pb, pd, depth, color, y_big);
	subDrawTriangle(targetTile, zBuffer, pc, pb, pd, depth, color, y_big);
}
void drawTriangle(tile& targetTile, dbuf& zBuffer, dRect const& viewPort,
	vector2 pa, vector2 pb, vector2 pc, double depth, dColor color)
{
	drawTriangle(targetTile, zBuffer, viewPort, (dVector2) pa, (dVector2) pb, (dVector2) pc, depth, color);
}
