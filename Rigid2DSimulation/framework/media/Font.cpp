#include "Font.h"

#include <cwchar>
#include <utility>
#include <vector>

Font::Font(int height) : height(height)
{
}

Font::~Font()
{
    Release();
}

void Font::Release()
{
    glyphs.clear();
    missingGlyph = {};
    baseline = 0;
    // 先销毁 DC，使字体不再被选中，然后删除字体并解除私有资源注册。
    if (deviceContext)
    {
        DeleteDC(deviceContext);
        deviceContext = nullptr;
    }
    if (fontHandle)
    {
        DeleteObject(fontHandle);
        fontHandle = nullptr;
    }
    if (!resourcePath.empty())
    {
        RemoveFontResourceExW(resourcePath.c_str(), FR_PRIVATE, nullptr);
        resourcePath.clear();
    }
}

bool Font::Initialize(std::wstring const& fileName)
{
    Release();
    if (height <= 0 || fileName.empty())
    {
        return false;
    }

    std::wstring path = fileName;
    if (!AddFontResourceExW(path.c_str(), FR_PRIVATE, nullptr))
    {
        wchar_t executablePath[32768];
        DWORD size = GetModuleFileNameW(nullptr, executablePath, 32768);
        if (size == 0 || size >= 32768 || fileName.find(L':') != std::wstring::npos ||
            fileName.front() == L'\\' || fileName.front() == L'/')
        {
            return false;
        }
        std::wstring directory(executablePath, size);
        auto separator = directory.find_last_of(L"\\/");
        if (separator == std::wstring::npos)
        {
            return false;
        }
        path = directory.substr(0, separator + 1) + fileName;
        if (!AddFontResourceExW(path.c_str(), FR_PRIVATE, nullptr))
        {
            return false;
        }
    }
    // 保存绝对路径，后续工作目录改变也能正确释放注册资源。
    wchar_t absolutePath[32768];
    DWORD pathSize = GetFullPathNameW(path.c_str(), 32768, absolutePath, nullptr);
    if (pathSize == 0 || pathSize >= 32768)
    {
        RemoveFontResourceExW(path.c_str(), FR_PRIVATE, nullptr);
        return false;
    }
    resourcePath.assign(absolutePath, pathSize);

    deviceContext = CreateCompatibleDC(nullptr);
    // 正高度指定字符单元高度，保持现有控件的 20 像素行高。
    fontHandle = CreateFontW(height, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                             DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS,
                             ANTIALIASED_QUALITY, DEFAULT_PITCH, L"Noto Sans SC");
    if (!deviceContext || !fontHandle)
    {
        Release();
        return false;
    }
    auto previousFont = SelectObject(deviceContext, fontHandle);
    wchar_t family[LF_FACESIZE]{};
    TEXTMETRICW metrics{};
    if (!previousFont || previousFont == HGDI_ERROR ||
        !GetTextFaceW(deviceContext, LF_FACESIZE, family) ||
        std::wcscmp(family, L"Noto Sans SC") != 0 ||
        !GetTextMetricsW(deviceContext, &metrics))
    {
        Release();
        return false;
    }
    baseline = (height - metrics.tmHeight) / 2 + metrics.tmAscent;
    if (!LoadGlyph(L'\uFFFD', missingGlyph) && !LoadGlyph(L'?', missingGlyph))
    {
        Release();
        return false;
    }
    missingGlyph.isMissing = true;
    return true;
}

bool Font::LoadGlyph(wchar_t character, FontGlyph& glyph) const
{
    if (!deviceContext)
    {
        return false;
    }
    WORD glyphIndex = 0;
    if (GetGlyphIndicesW(deviceContext, &character, 1, &glyphIndex, GGI_MARK_NONEXISTING_GLYPHS) == GDI_ERROR ||
        glyphIndex == 0xFFFF || glyphIndex == 0)
    {
        return false;
    }
    MAT2 transform{};
    transform.eM11.value = 1;
    transform.eM22.value = 1;
    GLYPHMETRICS metrics{};
    UINT format = GGO_GRAY8_BITMAP | GGO_GLYPH_INDEX;
    DWORD size = GetGlyphOutlineW(deviceContext, glyphIndex, format, &metrics, 0, nullptr, &transform);
    if (size == GDI_ERROR)
    {
        return false;
    }
    std::vector<BYTE> buffer(size);
    if (size != 0 && GetGlyphOutlineW(deviceContext, glyphIndex, format, &metrics, size, buffer.data(), &transform) == GDI_ERROR)
    {
        return false;
    }
    glyph.offset = {metrics.gmptGlyphOrigin.x, baseline - metrics.gmptGlyphOrigin.y};
    glyph.advance = metrics.gmCellIncX;
    // 空格等字形可以有排版宽度，但没有可读取的位图数据。
    if (size == 0)
    {
        return true;
    }
    glyph.bitmap = Tile(metrics.gmBlackBoxX, metrics.gmBlackBoxY);
    // GDI 灰度行按四字节对齐，覆盖率范围是 0..64，不是 0..255。
    size_t stride = (static_cast<size_t>(metrics.gmBlackBoxX) + 3) & ~size_t(3);
    for (int row = 0; row < glyph.bitmap.height; ++row)
    {
        for (int column = 0; column < glyph.bitmap.width; ++column)
        {
            glyph.bitmap.alphaValues[row * glyph.bitmap.width + column] =
                static_cast<BYTE>((buffer[row * stride + column] * 255 + 32) / 64);
        }
    }
    return true;
}

FontGlyph const& Font::GetGlyph(wchar_t character) const
{
    auto found = glyphs.find(character);
    if (found != glyphs.end())
    {
        return found->second;
    }
    FontGlyph glyph;
    if (!LoadGlyph(character, glyph))
    {
        glyph = missingGlyph;
        glyph.isMissing = true;
    }
    return glyphs.emplace(character, std::move(glyph)).first->second;
}
