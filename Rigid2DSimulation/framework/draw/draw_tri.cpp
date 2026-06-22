#include "draw_tri.h"

void subDrawTriangle(tile& dest, dbuf& zBuffer,
	dVector2 pa, dVector2 pb, dVector2 pc, double depth, dColor color, bool same_y)
{
	// ���� pa, pb, pc  �Ѿ��������ˡ�
	// ������б�ĳ������е�ʱ�������Ѻۣ���֪���ܲ����޸���
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
	// �������������д�úÿ��������������޸��Ҳ����׳� bug �ͺ��ˡ�
	// @todo:���Ǳʼ��е� SH �㷨

#define EdgeCut(a, c, x, y, sgn, borderLine)\
if (p##c.y sgn borderLine) { return; }/*��Сֵ�����߽�*/\
if (p##a.y sgn borderLine)/*���ֵ�����߽磬�и�*/ \
{\
	/*ac�߱��и�*/\
	dVector2 pac = p##c;\
	pac.y = borderLine;\
	pac.x += (p##a.x - p##c.x) * (borderLine - p##c.y) / (p##a.y - p##c.y);\
	if (pb.y sgn borderLine)\
	{\
		/*bc��Ҳ���и�õ�һ����c��pac��pbc��ɵ�������*/					\
		dVector2 p1 = p##c;\
		p1.y = borderLine;\
		p1.x += (pb.x - p##c.x) * (borderLine - p##c.y) / (p##b.y - p##c.y);\
		drawTriangle(targetTile, zBuffer, viewPort, pac, p1, p##c, depth, color);\
		return;/*�ڵݹ����һ������������ֽ����и���Ե��˽�������ͬ*/\
	}\
	else {\
		/*bc�߲����и�õ�һ����c��pac��pab��b��ɵ��ı���*/\
		dVector2 p1 = pb;\
		p1.y = borderLine;\
		p1.x += (p##a.x - pb.x) * (borderLine - pb.y) / (p##a.y - pb.y);\
		drawTriangle(targetTile, zBuffer, viewPort, pac, pb, p##c, depth, color);\
		drawTriangle(targetTile, zBuffer, viewPort, pac, pb, p1, depth, color);\
		return;\
	}\
}

	// ��֤x���������ǡ�cba����˳��
	// �����е��Ч���ظ��Ƚ��ˣ������ܵ�Ӱ�첻��
	if (pa.x < pb.x) swap(pa, pb);
	if (pa.x < pc.x) swap(pa, pc);
	if (pb.x < pc.x) swap(pb, pc);

	if (pa.x == pc.x) { return; } // ���������㲻���ܹ���
	int dx = pa.x - pc.x;

	// ����Ļ���и�ͼ��
	EdgeCut(a, c, y, x, > , (long long) viewPort.right() - 1);
	EdgeCut(c, a, y, x, < , (long long) viewPort.left());

	if (pa.y < pb.y) { swap(pa, pb); }
	if (pa.y < pc.y) { swap(pa, pc); }
	if (pb.y < pc.y) { swap(pb, pc); }
	if (pa.y == pc.y) { return; }
	int dy = pa.y - pc.y;
	EdgeCut(a, c, x, y, > , (long long) viewPort.bottom() - 1);
	EdgeCut(c, a, x, y, < , (long long) viewPort.top());

	// �����ǳ�����ֵ�ȶ��ԵĿ��ǽ���һ�·��򣬷�����������κܱ�Ļ�����֡�
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
	// @ques�����Ǻ�����������ʲôҪ������
	subDrawTriangle(targetTile, zBuffer, pa, pb, pd, depth, color, y_big);
	subDrawTriangle(targetTile, zBuffer, pc, pb, pd, depth, color, y_big);
}
void drawTriangle(tile& targetTile, dbuf& zBuffer, dRect const& viewPort,
	vector2 pa, vector2 pb, vector2 pc, double depth, dColor color)
{
	drawTriangle(targetTile, zBuffer, viewPort, (dVector2) pa, (dVector2) pb, (dVector2) pc, depth, color);
}

