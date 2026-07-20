#include "tile.h"

const dColor dColor::RED = dColor(255, 0, 0);
const dColor dColor::GREEN = dColor(0, 255, 0);
const dColor dColor::BLUE = dColor(0, 0, 255);
const dColor dColor::CYAN = dColor(0, 255, 255);
const dColor dColor::MAGENTA = dColor(255, 0, 255);
const dColor dColor::YELLOW = dColor(255, 255, 0);
const dColor dColor::WHITE = dColor(255, 255, 255);
const dColor dColor::BLACK = dColor(0, 0, 0);

dColor::dColor(wstring const& s)
{
	int i = 0;
	r = fetch_num(i, s);
	g = fetch_num(i, s);
	b = fetch_num(i, s);
}
wstring ToWString(dColor c)
{
	return L"(" + std::to_wstring(c.r) + L"," + std::to_wstring(c.g) + L"," + std::to_wstring(c.b) + L")";
}
void cover(dColor& bc, BYTE& ba, dColor fc, BYTE fa)
{
	// 目前还不会推导公式，应该有简化的空间。
	if (fa == 0) { return; }
	if (fa == 255) { bc = fc; ba = 255; return; }

	int a = 255 * (ba + fa) - ba * fa;
	int r = (bc.r * ba * (255 - fa) + fc.r * fa * 255) / a;
	int g = (bc.g * ba * (255 - fa) + fc.g * fa * 255) / a;
	int b = (bc.b * ba * (255 - fa) + fc.b * fa * 255) / a; a /= 255;
	bc = dColor(r, g, b); ba = a;
}

tile::tile(int w, int h, dColor color, BYTE a) :
	width(w), height(h), colors(area(), color), as(area(), a)
{
}
tile::tile(int w, int h, tile const& src, dRect vp_src) : tile(w, h)
{
	for (int i = (0), iEnd = int(w); i < iEnd; ++i) for (int j = (0), jEnd = int(h); j < jEnd; ++j)
	{
		dVector2 d_pnt(i, j);
		dVector2 s_pnt = d_pnt * dVector2(vp_src.w, vp_src.h) / dVector2(w, h);

		int dp = d_pnt.y * w + d_pnt.x;
		int sp = (s_pnt.y + vp_src.top()) * src.width + s_pnt.x + vp_src.left();
		colors[dp] = src.colors[sp]; as[dp] = src.as[sp];
	}
}
void tile::save(wstring const& fileName) const
{
	FILE* file = wfopen(fileName, L"wb");
	if (!file) { return; }
	fwrite(&(width), sizeof(width), 1, file);
	fwrite(&(height), sizeof(height), 1, file);
	fwrite(&colors[0], sizeof(*(&colors[0])), area(), file);
	fwrite(&as[0], sizeof(*(&as[0])), area(), file);
	fclose(file);
}
tile::tile(wstring const& fileName, bool* ok)
{
	FILE* file = wfopen(fileName, L"rb");
	if (ok) { *ok = file; }
	if (!file) { return; }
	fread(&(width), sizeof(width), 1, file);
	fread(&(height), sizeof(height), 1, file);
	colors.resize(width * height); as.resize(width * height);
	fread(&colors[0], sizeof(*(&colors[0])), area(), file);
	fread(&as[0], sizeof(*(&as[0])), area(), file);
	fclose(file);
}

int tile::area() const { return width * height; }
dRect tile::rect() const { return { width, height }; }
void tile::fill(dColor color, BYTE a)
{
	for (int i = (0), iEnd = int(area()); i < iEnd; ++i) { colors[i] = color; }
	memset(as.data(), a, area());
}

