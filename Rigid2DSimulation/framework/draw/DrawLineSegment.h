#pragma once
#include "Vector3.h"
#include "Tile.h"


// 仅在整型向量绘制中调用
bool preDrawLineSegment(DVector2& pa, DVector2& pb, DRect viewPort);

void drawLineSegment(Tile& targetTile, DepthBuffer& zBuffer,
	DVector2 pa, DVector2 pb, double d, DRect viewPort, Color color);

// 浮点向量的重载
void drawLineSegment(Tile& targetTile, DepthBuffer& zBuffer,
	Vector2 pa, Vector2 pb, double depth, DRect viewPort, Color color);

