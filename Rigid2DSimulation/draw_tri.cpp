#include "draw_tri.h"

void subDrawTriangle(tile& dest, dbuf& zBuffer,
	dVector2 pa, dVector2 pb, dVector2 pc, double depth, dColor color, bool same_y) {
	// ���� pa, pb, pc  �Ѿ��������ˡ�
	// ������б�ĳ������е�ʱ�������Ѻۣ���֪���ܲ����޸���
	if (pa.y == pb.y)  return; 
	int sy = pb.y > pa.y ? 1 : -1;
	int sx = pc.x > pb.x ? 1 : -1;
	for (int y = pa.y; y != pb.y + sy; y += sy) 
	{
		int xb = pa.x * (pb.y - y) + pb.x * (y - pa.y); xb /= (pb.y - pa.y);
		int xc = pa.x * (pb.y - y) + pc.x * (y - pa.y); xc /= (pb.y - pa.y);
		for (int x = xb; x != xc + sx; x += sx) {
			int dp = same_y ?
				y * dest.width + x : x * dest.width + y;
			if (zBuffer[dp] <= depth) { zBuffer[dp] = depth; dest.colors[dp] = color; }
		}
	}
}
void draw_tri(tile& dest, dbuf& ds, dRect const& vp,
	dVector2 pa, dVector2 pb, dVector2 pc, double d, dColor col) {
	// �������������д�úÿ������������޸��Ҳ����׳� bug �ͺ��ˡ�

#define TMP(a, c, x, y, sgn, bd)\
if (p##c.y sgn bd) { return; }\
if (p##a.y sgn bd) {\
	dVector2 p0 = p##c; p0.y = bd;\
	p0.x += (p##a.x - p##c.x) * (bd - p##c.y) / (p##a.y - p##c.y);\
	if (pb.y sgn bd) {\
		dVector2 p1 = p##c; p1.y = bd;\
		p1.x += (pb.x - p##c.x) * (bd - p##c.y) / (p##b.y - p##c.y);\
		draw_tri(dest, ds, vp, p0, p1, p##c, d, col); return;\
	}\
	else {\
		dVector2 p1 = pb; p1.y = bd;\
		p1.x += (p##a.x - pb.x) * (bd - pb.y) / (p##a.y - pb.y);\
		draw_tri(dest, ds, vp, p0, pb, p##c, d, col);\
		draw_tri(dest, ds, vp, p0, pb, p1, d, col); return;\
	}\
}

	// �����е��Ч���ظ��Ƚ��ˣ������ܵ�Ӱ�첻��
	if (pa.x < pb.x) { swap(pa, pb); }
	if (pa.x < pc.x) { swap(pa, pc); }
	if (pb.x < pc.x) { swap(pb, pc); }
	if (pa.x == pc.x) { return; }
	int dx = pa.x - pc.x;
	TMP(a, c, y, x, > , (long long)vp.right() - 1);
	TMP(c, a, y, x, < , (long long)vp.left());

	if (pa.y < pb.y) { swap(pa, pb); }
	if (pa.y < pc.y) { swap(pa, pc); }
	if (pb.y < pc.y) { swap(pb, pc); }
	if (pa.y == pc.y) { return; }
	int dy = pa.y - pc.y;
	TMP(a, c, x, y, > , (long long)vp.bottom() - 1);
	TMP(c, a, x, y, < , (long long)vp.top());

	// �����ǳ�����ֵ�ȶ��ԵĿ��ǽ���һ�·��򣬷�����������κܱ�Ļ�����֡�
	bool y_big = dy > dx;
	if (!y_big) {
		if (pa.x < pb.x) { swap(pa, pb); }
		if (pa.x < pc.x) { swap(pa, pc); }
		if (pb.x < pc.x) { swap(pb, pc); }
		swap(pa.x, pa.y); swap(pb.x, pb.y); swap(pc.x, pc.y);
	}
	dVector2 pd = pb;
	pd.x = pa.x * (pb.y - pc.y) + pc.x * (pa.y - pb.y); pd.x /= pa.y - pc.y;
	subDrawTriangle(dest, ds, pa, pb, pd, d, col, y_big);
	subDrawTriangle(dest, ds, pc, pb, pd, d, col, y_big);
}
void draw_tri(tile& dest, dbuf& ds, dRect const& vp,
	vector2 pa, vector2 pb, vector2 pc, double d, dColor col) {
	draw_tri(dest, ds, vp, (dVector2)pa, (dVector2)pb, (dVector2)pc, d, col);
}
