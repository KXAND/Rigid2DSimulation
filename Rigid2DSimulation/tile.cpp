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
wstring tw(dColor c)
{
	return L"(" + tw(c.r) + L"," + tw(c.g) + L"," + tw(c.b) + L")";
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
	rep(i, 0, w) rep(j, 0, h)
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
	FILE* f = wfopen(fileName, L"wb");
	if (!f) { return; }
	fwt(width); fwt(height);
	fwts(&colors[0], area()); fwts(&as[0], area()); fclose(f);
}
tile::tile(wstring const& fileName, bool* ok)
{
	FILE* f = wfopen(fileName, L"rb");
	if (ok) { *ok = f; }
	if (!f) { return; }
	frd(width); frd(height);
	colors.resize(width * height); as.resize(width * height);
	frds(&colors[0], area()); frds(&as[0], area()); fclose(f);
}

int tile::area() const { return width * height; }
dRect tile::rect() const { return { width, height }; }
void tile::fill(dColor color, BYTE a)
{
	rep(i, 0, area()) { colors[i] = color; }
	memset(as.data(), a, area());
}
