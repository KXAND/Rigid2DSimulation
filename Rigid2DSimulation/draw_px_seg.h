#pragma once
#include "vector3.h"
#include "tile.h"


// 仅在整型向量绘制中调用
bool preDrawLineSegment(dVector2& pa, dVector2& pb, dRect viewPort);

void drawLineSegment(tile& targetTile, dbuf& zBuffer,
	dVector2 pa, dVector2 pb, double d, dRect viewPort, dColor color);

// 浮点向量的重载
void drawLineSegment(tile& targetTile, dbuf& zBuffer,
	vector2 pa, vector2 pb, double depth, dRect viewPort, dColor color);
