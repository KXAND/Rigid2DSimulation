#include "Tile.h"

const Color Color::RED = Color(255, 0, 0);
const Color Color::GREEN = Color(0, 255, 0);
const Color Color::BLUE = Color(0, 0, 255);
const Color Color::CYAN = Color(0, 255, 255);
const Color Color::MAGENTA = Color(255, 0, 255);
const Color Color::YELLOW = Color(255, 255, 0);
const Color Color::WHITE = Color(255, 255, 255);
const Color Color::BLACK = Color(0, 0, 0);

Color::Color(wstring const& s)
{
	int i = 0;
	r = FetchNumber(i, s);
	g = FetchNumber(i, s);
	b = FetchNumber(i, s);
}
wstring ToWString(Color c)
{
	return L"(" + std::to_wstring(c.r) + L"," + std::to_wstring(c.g) + L"," + std::to_wstring(c.b) + L")";
}
void BlendColor(Color& bc, BYTE& ba, Color fc, BYTE fa)
{
	// 目前还不会推导公式，应该有简化的空间。
	if (fa == 0) { return; }
	if (fa == 255) { bc = fc; ba = 255; return; }

	int a = 255 * (ba + fa) - ba * fa;
	int r = (bc.r * ba * (255 - fa) + fc.r * fa * 255) / a;
	int g = (bc.g * ba * (255 - fa) + fc.g * fa * 255) / a;
	int b = (bc.b * ba * (255 - fa) + fc.b * fa * 255) / a; a /= 255;
	bc = Color(r, g, b); ba = a;
}

Tile::Tile(int w, int h, Color color, BYTE a) :
	width(w), height(h), colors(GetArea(), color), as(GetArea(), a)
{
}
Tile::Tile(int w, int h, Tile const& src, DRect sourceViewport) : Tile(w, h)
{
	for (int i = (0), iEnd = int(w); i < iEnd; ++i) for (int j = (0), jEnd = int(h); j < jEnd; ++j)
	{
		DVector2 targetPoint(i, j);
		DVector2 sourcePoint = targetPoint * DVector2(sourceViewport.width, sourceViewport.height) / DVector2(w, h);

		int targetPixelIndex = targetPoint.y * w + targetPoint.x;
		int sourcePixelIndex = (sourcePoint.y + sourceViewport.GetTop()) * src.width + sourcePoint.x + sourceViewport.GetLeft();
		colors[targetPixelIndex] = src.colors[sourcePixelIndex]; as[targetPixelIndex] = src.as[sourcePixelIndex];
	}
}
void Tile::Save(wstring const& fileName) const
{
	FILE* file = OpenFile(fileName, L"wb");
	if (!file) { return; }
	fwrite(&(width), sizeof(width), 1, file);
	fwrite(&(height), sizeof(height), 1, file);
	fwrite(&colors[0], sizeof(*(&colors[0])), GetArea(), file);
	fwrite(&as[0], sizeof(*(&as[0])), GetArea(), file);
	fclose(file);
}
Tile::Tile(wstring const& fileName, bool* ok)
{
	FILE* file = OpenFile(fileName, L"rb");
	if (ok) { *ok = file; }
	if (!file) { return; }
	fread(&(width), sizeof(width), 1, file);
	fread(&(height), sizeof(height), 1, file);
	colors.resize(width * height); as.resize(width * height);
	fread(&colors[0], sizeof(*(&colors[0])), GetArea(), file);
	fread(&as[0], sizeof(*(&as[0])), GetArea(), file);
	fclose(file);
}

int Tile::GetArea() const { return width * height; }
DRect Tile::Rect() const { return { width, height }; }
void Tile::Fill(Color color, BYTE a)
{
	for (int i = (0), iEnd = int(GetArea()); i < iEnd; ++i) { colors[i] = color; }
	memset(as.data(), a, GetArea());
}

