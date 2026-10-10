#pragma once
#include "Tile.h"

wchar_t extern chineseCharacters[];
wchar_t extern numbers[];
wchar_t extern normalSymbols[];
wchar_t extern uppercaseLetters[];
wchar_t extern lowercaseLetters[];
wchar_t extern additionalChineseCharacters[];

// 中文字体规格：画布：600x600，粗细：40，保存后：60x60。
// 由于历史原因，颜色一般是: (0,255,0)。
class Font
{
public:
    int height = 0;
    // 已成功加载的中文字符，区别于全局待加载字符表。
    wstring loadedChineseCharacters;
    Tile missing;
    map<wchar_t, Tile> glyphs;

    Font();
    Font(int height);

    Tile const& operator[](wchar_t character) const;
    // 严格来说用单独的方法更好，但是这样很方便。
    wstring GenerateRandomText(int count) const;

    bool LoadFont(wstring const& name, wchar_t character);
    bool Initialize();
};
