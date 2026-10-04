#pragma once
#include "Vector2.h"
#include "Tile.h"
/// <summary>
/// 最底层的Tile，用于SceneCanvas，无需检查深度
/// </summary>
/// <param name="targetTile"></param>
/// <param name="topLeft"></param>
/// <param name="sourceTile"></param>
void DrawTileRaw(Tile& targetTile, DVector2 topLeft, Tile const& sourceTile);

/// <summary>
/// 带有深度的绘制 Tile绘制
/// 这里 targetViewport 允许宽度为0等异常情况，但是 sourceViewport 不允许。如何加以限制还没想好。
/// </summary>
void DrawTile(Tile& targetTile, DepthBuffer& depthBuffer, double depth, DVector2 topLeft, DRect targetViewport, Tile const& sourceTile, DRect sourceViewport);
/// <summary>
/// 弃用？
/// </summary>
/// <param name="point"></param>
/// <param name="topLeft"></param>
/// <param name="sourceTile"></param>
/// <param name="sourceViewport"></param>
/// <returns></returns>
bool HitTestTile(DVector2 point, DVector2 topLeft, Tile const& sourceTile, DRect sourceViewport);
