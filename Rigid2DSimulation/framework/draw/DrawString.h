#pragma once
#include "Font.h"

void DrawCharacter(Tile& targetTile, DepthBuffer& depthBuffer, double depth,
                   DVector2 topLeft, DRect viewport, Tile const& sourceTile, Color color);
// w 为 0 时代表不换行。
void DrawString(Tile& targetTile, DepthBuffer& depthBuffer, double depth,
                wstring const& text, Color color, Font const& font,
                DVector2& currentTopLeft, int& currentX, int maxWidth, DRect viewport);
void DrawString(Tile& targetTile, DepthBuffer& depthBuffer, double depth,
                wstring const& text, Color color, Font const& font, DVector2 topLeft,
                int maxWidth, DRect viewport);

DVector2 GetStringSize(wstring const& text, Font const& font, int maxWidth,
                       int* lastLineWidth = NULL);
// 如果不想考虑高的话，就设置 p.y = 0。
int GetStringIndexAtPosition(wstring const& text, Font const& font,
                             DVector2 position, int maxWidth);
