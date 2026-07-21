#pragma once
#include "vector3.h"
#include "tile.h"


// 仅在整型向量绘制中调用
bool preDrawLineSegment(DVector2& pa, DVector2& pb, DRect viewPort);

void drawLineSegment(tile& targetTile, dbuf& zBuffer,
	DVector2 pa, DVector2 pb, double d, DRect viewPort, dColor color);

// 浮点向量的重载
void drawLineSegment(tile& targetTile, dbuf& zBuffer,
	Vector2 pa, Vector2 pb, double depth, DRect viewPort, dColor color);

