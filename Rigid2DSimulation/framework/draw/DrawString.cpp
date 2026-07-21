#include "DrawString.h"
#include "Font.h"

void DrawCharacter(Tile& targetTile, DepthBuffer& depthBuffer, double depth,
    DVector2 topLeft, DRect viewport, Tile const& sourceTile, Color color)
{
    int left = max(viewport.GetLeft(), topLeft.x);
    int right = min(viewport.GetRight(), topLeft.x + sourceTile.width);
    int top = max(viewport.GetTop(), topLeft.y);
    int bottom = min(viewport.GetBottom(), topLeft.y + sourceTile.height);
    if (left >= right || top >= bottom)
    {
        return;
    }

    for (int row = top; row < bottom; ++row)
    {
        int targetPixelIndex = row * targetTile.width + left;
        int sourcePixelIndex = (row - topLeft.y) * sourceTile.width + left - topLeft.x;
        for (int column = left; column < right; ++column)
        {
            if (depthBuffer[targetPixelIndex] <= depth && sourceTile.alphaValues[sourcePixelIndex] != 0)
            {
                depthBuffer[targetPixelIndex] = depth;
                targetTile.colors[targetPixelIndex] = color;
            }
            targetPixelIndex++;
            sourcePixelIndex++;
        }
    }
}

void DrawString(Tile& targetTile, DepthBuffer& depthBuffer, double depth,
    wstring const& text, Color color, Font const& font,
    DVector2& currentTopLeft, int& currentX, int maxWidth, DRect viewport)
{
    for (wchar_t character : text)
    {
        if (character == '\n')
        {
            currentTopLeft.y += font.height;
            currentX = 0;
        }
        else
        {
            Tile const& glyph = font[character];
            if (maxWidth != 0 && currentX > 0 && currentX + glyph.width > maxWidth)
            {
                currentTopLeft.y += font.height;
                currentX = 0;
            }

            DVector2 characterTopLeft = currentTopLeft + DVector2(currentX, 0);
            DrawCharacter(targetTile, depthBuffer, depth, characterTopLeft, viewport, glyph, color);
            currentX += glyph.width;
        }
    }
}

void DrawString(Tile& targetTile, DepthBuffer& depthBuffer, double depth,
    wstring const& text, Color color, Font const& font, DVector2 topLeft,
    int maxWidth, DRect viewport)
{
    int currentX = 0;
    DrawString(targetTile, depthBuffer, depth, text, color, font, topLeft, currentX, maxWidth, viewport);
}

DVector2 GetStringSize(wstring const& text, Font const& font, int maxWidth,
    int* lastLineWidth)
{
    int currentX = 0;
    int width = 0;
    int height = font.height;
    for (wchar_t character : text)
    {
        if (character == L'\n')
        {
            width = max(width, currentX);
            height += font.height;
            currentX = 0;
        }
        else
        {
            Tile const& glyph = font[character];
            if (maxWidth != 0 && currentX > 0 && currentX + glyph.width > maxWidth)
            {
                width = max(width, currentX);
                height += font.height;
                currentX = 0;
            }

            // 下面两个顺序可别乱动啊。
            currentX += glyph.width;
            width = max(width, currentX);
        }
    }
    if (lastLineWidth)
    {
        *lastLineWidth = currentX;
    }
    return { width, height };
}

int GetStringIndexAtPosition(wstring const& text, Font const& font,
    DVector2 position, int maxWidth)
{
    int currentX = 0;
    int currentY = 0;
    if (0 > position.y)
    {
        return 0;
    }

    for (int index = 0, textLength = int(text.size()); index < textLength; ++index)
    {
        wchar_t character = text[index];
        if (character == L'\n')
        {
            currentY += font.height;
            currentX = 0;
            if (currentY > position.y)
            {
                return index;
            }
        }
        else
        {
            Tile const& glyph = font[character];
            if (maxWidth != 0 && currentX > 0 && currentX + glyph.width > maxWidth)
            {
                currentY += font.height;
                currentX = 0;
                if (currentY > position.y)
                {
                    return index;
                }
            }
            currentX += glyph.width;
            if (currentX > position.x && currentY + font.height > position.y)
            {
                return currentX - position.x > glyph.width / 2 ? index : index + 1;
            }
        }
    }
    return int(text.size());
}
