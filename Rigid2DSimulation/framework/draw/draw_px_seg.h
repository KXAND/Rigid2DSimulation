#pragma once
#include "vector3.h"
#include "tile.h"


// �����������������е���
bool preDrawLineSegment(dVector2& pa, dVector2& pb, dRect viewPort);

void drawLineSegment(tile& targetTile, dbuf& zBuffer,
	dVector2 pa, dVector2 pb, double d, dRect viewPort, dColor color);

// ��������������
void drawLineSegment(tile& targetTile, dbuf& zBuffer,
	vector2 pa, vector2 pb, double depth, dRect viewPort, dColor color);

