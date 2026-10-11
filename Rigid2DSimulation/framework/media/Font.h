#pragma once

#include <map>
#include <string>

#include "Tile.h"

struct FontGlyph
{
    Tile bitmap;
    DVector2 offset;
    int advance = 0;
    bool isMissing = false;
};

class Font
{
public:
    int const height;

    explicit Font(int height = 20);
    ~Font();
    Font(Font const&) = delete;
    Font& operator=(Font const&) = delete;

    bool Initialize(std::wstring const& fileName = L"fonts/NotoSansSC.ttf");
    FontGlyph const& GetGlyph(wchar_t character) const;

private:
    HDC deviceContext = nullptr;
    HFONT fontHandle = nullptr;
    std::wstring resourcePath;
    int baseline = 0;
    FontGlyph missingGlyph;
    // 单线程 UI 的逻辑只读缓存；字体和字号在缓存存续期间不变。
    mutable std::map<wchar_t, FontGlyph> glyphs;

    void Release();
    bool LoadGlyph(wchar_t character, FontGlyph& glyph) const;
};
