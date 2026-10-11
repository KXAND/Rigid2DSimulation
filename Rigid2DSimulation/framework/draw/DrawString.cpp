#include "DrawString.h"
#include "Font.h"

void DrawCharacter(Tile& targetTile, DepthBuffer& depthBuffer, double depth,
                   DVector2 topLeft, DRect viewport, Tile const& sourceTile, Color color)
{
    int left = max(0, max(viewport.GetLeft(), topLeft.x));
    int right = min(targetTile.width, min(viewport.GetRight(), topLeft.x + sourceTile.width));
    int top = max(0, max(viewport.GetTop(), topLeft.y));
    int bottom = min(targetTile.height, min(viewport.GetBottom(), topLeft.y + sourceTile.height));
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
                // screen 的呈现不读取 alpha，直接将字形覆盖率混入现有屏幕颜色。
                int alpha = sourceTile.alphaValues[sourcePixelIndex];
                auto& background = targetTile.colors[targetPixelIndex];
                background = Color(
                    (color.red * alpha + background.red * (255 - alpha) + 127) / 255,
                    (color.green * alpha + background.green * (255 - alpha) + 127) / 255,
                    (color.blue * alpha + background.blue * (255 - alpha) + 127) / 255);
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
            auto const& glyph = font.GetGlyph(character);
            if (maxWidth != 0 && currentX > 0 && currentX + glyph.advance > maxWidth)
            {
                currentTopLeft.y += font.height;
                currentX = 0;
            }

            DVector2 characterTopLeft = currentTopLeft + DVector2(currentX, 0) + glyph.offset;
            DrawCharacter(targetTile, depthBuffer, depth, characterTopLeft, viewport, glyph.bitmap, color);
            currentX += glyph.advance;
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
            auto const& glyph = font.GetGlyph(character);
            if (maxWidth != 0 && currentX > 0 && currentX + glyph.advance > maxWidth)
            {
                width = max(width, currentX);
                height += font.height;
                currentX = 0;
            }

            // 下面两个顺序可别乱动啊。
            currentX += glyph.advance;
            width = max(width, currentX);
        }
    }
    if (lastLineWidth)
    {
        *lastLineWidth = currentX;
    }
    return {width, height};
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
            auto const& glyph = font.GetGlyph(character);
            if (maxWidth != 0 && currentX > 0 && currentX + glyph.advance > maxWidth)
            {
                currentY += font.height;
                currentX = 0;
                if (currentY > position.y)
                {
                    return index;
                }
            }
            currentX += glyph.advance;
            if (currentX > position.x && currentY + font.height > position.y)
            {
                return currentX - position.x > glyph.advance / 2 ? index : index + 1;
            }
        }
    }
    return int(text.size());
}
