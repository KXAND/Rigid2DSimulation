#pragma once
#include "vector2.h"
#include "tile.h"
/// <summary>
/// 最底层的Tile，用于Background，无需检查深度
/// </summary>
/// <param name="dest"></param>
/// <param name="tl"></param>
/// <param name="src"></param>
void drawBasedTile(tile& dest, dVector2 tl, tile const& src);

/// <summary>
/// 带有深度的绘制 Tile绘制
/// 这里 vp_dest 允许宽度为0等异常情况，但是 vp_src 不允许。如何加以限制还没想好。
/// </summary>
void drawTile(tile& dest, dbuf& ds, double dep, dVector2 tl, dRect vp_dest, tile const& src, dRect vp_src);
/// <summary>
/// 弃用？
/// </summary>
/// <param name="p"></param>
/// <param name="tl"></param>
/// <param name="src"></param>
/// <param name="vp_src"></param>
/// <returns></returns>
bool hitTile(dVector2 p, dVector2 tl, tile const& src, dRect vp_src);
