#include "draw_comp.h"
#include "draw_geo.h"

void drawRectangleWithBorderRaw(tile& targetTile, 
	dVector2 topLeft, int width, int height, dRect viewPort, dColor color, dColor borderColor)
{
	// ����Ϊԭ�㣬��������

	// �ڲ�
	drawRectangleRaw(targetTile, topLeft, width, height, viewPort, color);

	dVector2 topRight = topLeft + dVector2(width, 0);
	dVector2 bottomLeft = topLeft + dVector2(0, height);

	// ����ɫ
	drawRectangleRaw(targetTile, topLeft, width, 1, viewPort, borderColor);
	drawRectangleRaw(targetTile, bottomLeft, width, 1, viewPort, borderColor);
	drawRectangleRaw(targetTile, topLeft, 1, height, viewPort, borderColor);
	drawRectangleRaw(targetTile, topRight, 1, height, viewPort, borderColor);
}

// ����ȵľ���
void drawRectangleWithBorder(tile& targetTile, dbuf& zBuffer, double depth,
	dVector2 topLeft, int width, int height, dRect viewPort, dColor color, dColor borderColor)
{
	// �����ڲ�����
	drawRectangle(targetTile, zBuffer, depth, topLeft, width, height, viewPort, color);


	// ���ƾ������
	dVector2 topRight = topLeft + dVector2(width, 0);
	dVector2 bottomLeft = topLeft + dVector2(0, height);

	drawRectangle(targetTile, zBuffer, depth, topLeft, width, 1, viewPort, color);
	drawRectangle(targetTile, zBuffer, depth, bottomLeft, width, 1, viewPort, color);
	drawRectangle(targetTile, zBuffer, depth, topLeft, 1, height, viewPort, color);
	drawRectangle(targetTile, zBuffer, depth, topRight, 1, height, viewPort, color);
}

