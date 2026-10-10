#include "Font.h"

Font::Font() : Font(20) {}
Font::Font(int height) : height(height) {}
Tile const& Font::operator[](wchar_t character) const
{
    auto it = glyphs.find(character);
    return it == glyphs.end() ? missing : it->second;
}
wstring Font::GenerateRandomText(int count) const
{
    wstring text;
    for (int i = (0), iEnd = int(count); i < iEnd; ++i)
    {
        text += loadedChineseCharacters[GenerateRandomInt(loadedChineseCharacters.size())];
    }
    return text;
}

bool Font::LoadFont(wstring const& name, wchar_t character)
{
    wstring path = L"./fonts/" + name + L".tile";
    bool isLoaded = false;
    Tile sourceTile(path, &isLoaded);
    if (isLoaded)
    {
        Tile scaledTile = Tile(sourceTile.width * height / sourceTile.height, height, sourceTile, sourceTile.Rect());
        glyphs[character] = scaledTile;
        return true;
    }
    else
    {
        return false;
    }
}
bool Font::Initialize()
{
    wstring normalCharacters = (wstring)numbers + normalSymbols + lowercaseLetters;
    wstring uppercaseCharacters = (wstring)uppercaseLetters;
    wstring chineseCharacterSet = (wstring)chineseCharacters + additionalChineseCharacters;

    LoadFont(L"less", L'<');
    LoadFont(L"quote", L'\"');
    LoadFont(L"blank", L' ');
    LoadFont(L"slash", L'/');
    LoadFont(L"colon", L':');
    LoadFont(L"greater", L'>');
    LoadFont(L"vertical", L'|');
    LoadFont(L"asterisk", L'*');
    LoadFont(L"question", L'?');
    LoadFont(L"backslash", L'\\');
    for (auto character : normalCharacters)
    {
        LoadFont({character}, character);
    }
    for (auto character : uppercaseCharacters)
    {
        LoadFont({character, character}, character);
    }
    for (auto character : chineseCharacterSet)
    {
        if (LoadFont({character}, character))
        {
            loadedChineseCharacters.push_back(character);
        }
    }

    wstring path = L"./fonts/missing.tile";
    bool isLoaded = false;
    Tile sourceTile(path, &isLoaded);
    if (isLoaded)
    {
        Tile scaledTile = Tile(sourceTile.width * height / sourceTile.height, height, sourceTile, sourceTile.Rect());
        missing = scaledTile;
        return true;
    }
    else
    {
        return false;
    }
}
