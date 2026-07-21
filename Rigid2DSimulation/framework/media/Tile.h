#pragma once
#include "Vector3.h"
struct Color
{
	BYTE b = 0, g = 0, r = 0;

	static const Color RED;
	static const Color GREEN;
	static const Color BLUE;
	static const Color CYAN;
	static const Color YELLOW;
	static const Color MAGENTA;
	static const Color WHITE;
	static const Color BLACK;


	Color() = default;
	Color(BYTE r, BYTE g, BYTE b) : r(r), g(g), b(b) { }
	explicit Color(BYTE a) : Color(a, a, a) { }
	explicit Color(wstring const& s);
	explicit Color(Vector3 c)
	{
		r = Clamp<int>(0, 255, (int) c.r() * 255);
		g = Clamp<int>(0, 255, (int) c.g() * 255);
		b = Clamp<int>(0, 255, (int) c.b() * 255);
	}
	// 允许转换为 Vector3，禁止显式操作
	explicit operator Vector3() const { return Vector3(r, g, b) / 255; }
};
inline bool operator==(Color a, Color b)
{
	return memcmp(&a, &b, sizeof(Color)) == 0;
}
inline bool operator!=(Color a, Color b) { return !(a == b); }
wstring ToWString(Color c);
void cover(Color& bc, BYTE& ba, Color fc, BYTE fa);

struct Tile
{
	int width = 0, height = 0;
	vector<Color> colors;
	vector<BYTE> as;

	Tile() = default;
	Tile(int w, int h, Color c = {}, BYTE a = 0);
	Tile(int w, int h, Tile const& src, DRect vp_src);// src 不能为空。
	Tile(wstring const& fileName, bool* ok = NULL);
	void save(wstring const& fileName) const;

	int area() const;
	DRect Rect() const;
	void fill(Color color, BYTE a = 255);
};
typedef vector<double> DepthBuffer;

