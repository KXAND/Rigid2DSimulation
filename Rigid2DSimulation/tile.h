#pragma once
#include "vector3.h"
struct dColor
{
	BYTE b = 0, g = 0, r = 0;

	static const dColor RED;
	static const dColor GREEN;
	static const dColor BLUE;
	static const dColor CYAN;
	static const dColor YELLOW;
	static const dColor YELLOW;
	static const dColor MAGENTA;
	static const dColor WHITE;
	static const dColor BLACK;


	dColor() = default;
	dColor(BYTE r, BYTE g, BYTE b) : r(r), g(g), b(b) { }
	explicit dColor(BYTE a) : dColor(a, a, a) { }
	explicit dColor(wstring const& s);
	explicit dColor(vector3 c)
	{
		r = clamp<int>(0, 255, (int) c.r() * 255);
		g = clamp<int>(0, 255, (int) c.g() * 255);
		b = clamp<int>(0, 255, (int) c.b() * 255);
	}
	// 允许转换为 vector3，禁止显式操作
	explicit operator vector3() const { return vector3(r, g, b) / 255; }
};
inline bool operator==(dColor a, dColor b)
{
	return memcmp(&a, &b, sizeof(dColor)) == 0;
}
inline bool operator!=(dColor a, dColor b) { return !(a == b); }
wstring tw(dColor c);
void cover(dColor& bc, BYTE& ba, dColor fc, BYTE fa);

struct tile
{
	int width = 0, height = 0;
	vector<dColor> colors;
	vector<BYTE> as;

	tile() = default;
	tile(int w, int h, dColor c = {}, BYTE a = 0);
	tile(int w, int h, tile const& src, dRect vp_src);// src 不能为空。
	tile(wstring const& fileName, bool* ok = NULL);
	void save(wstring const& fileName) const;

	int area() const;
	dRect rect() const;
	void fill(dColor color, BYTE a = 255);
};
typedef vector<double> dbuf;
