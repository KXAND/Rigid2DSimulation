#pragma once
#include "Vector3.h"
#include "Tile.h"


// 仅在整型向量绘制中调用
bool ClipLineSegment(DVector2& firstPoint, DVector2& secondPoint, DRect viewPort);

void DrawLineSegment(Tile& targetTile, DepthBuffer& zBuffer,
	DVector2 firstPoint, DVector2 secondPoint, double depth, DRect viewPort, Color color);

// 浮点向量的重载
void DrawLineSegment(Tile& targetTile, DepthBuffer& zBuffer,
	Vector2 firstPoint, Vector2 secondPoint, double depth, DRect viewPort, Color color);

