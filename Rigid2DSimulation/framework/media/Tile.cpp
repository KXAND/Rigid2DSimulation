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
void cover(Color& bc, BYTE& ba, Color fc, BYTE fa)
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
	width(w), height(h), colors(area(), color), as(area(), a)
{
}
Tile::Tile(int w, int h, Tile const& src, DRect vp_src) : Tile(w, h)
{
	for (int i = (0), iEnd = int(w); i < iEnd; ++i) for (int j = (0), jEnd = int(h); j < jEnd; ++j)
	{
		DVector2 d_pnt(i, j);
		DVector2 s_pnt = d_pnt * DVector2(vp_src.w, vp_src.h) / DVector2(w, h);

		int dp = d_pnt.y * w + d_pnt.x;
		int sp = (s_pnt.y + vp_src.GetTop()) * src.width + s_pnt.x + vp_src.GetLeft();
		colors[dp] = src.colors[sp]; as[dp] = src.as[sp];
	}
}
void Tile::save(wstring const& fileName) const
{
	FILE* file = OpenFile(fileName, L"wb");
	if (!file) { return; }
	fwrite(&(width), sizeof(width), 1, file);
	fwrite(&(height), sizeof(height), 1, file);
	fwrite(&colors[0], sizeof(*(&colors[0])), area(), file);
	fwrite(&as[0], sizeof(*(&as[0])), area(), file);
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
	fread(&colors[0], sizeof(*(&colors[0])), area(), file);
	fread(&as[0], sizeof(*(&as[0])), area(), file);
	fclose(file);
}

int Tile::area() const { return width * height; }
DRect Tile::Rect() const { return { width, height }; }
void Tile::fill(Color color, BYTE a)
{
	for (int i = (0), iEnd = int(area()); i < iEnd; ++i) { colors[i] = color; }
	memset(as.data(), a, area());
}

