#pragma once
#include "Vector3.h"
struct Color
{
	BYTE blue = 0, green = 0, red = 0;

	static const Color kRed;
	static const Color kGreen;
	static const Color kBlue;
	static const Color kCyan;
	static const Color kYellow;
	static const Color kMagenta;
	static const Color kWhite;
	static const Color kBlack;


	Color() = default;
	Color(BYTE red, BYTE green, BYTE blue) : red(red), green(green), blue(blue) { }
	explicit Color(BYTE alpha) : Color(alpha, alpha, alpha) { }
	explicit Color(wstring const& text);
	explicit Color(Vector3 color)
	{
		red = Clamp<int>(0, 255, (int) color.GetRed() * 255);
		green = Clamp<int>(0, 255, (int) color.GetGreen() * 255);
		blue = Clamp<int>(0, 255, (int) color.GetBlue() * 255);
	}
	// 允许转换为 Vector3，禁止显式操作
	explicit operator Vector3() const { return Vector3(red, green, blue) / 255; }
};
inline bool operator==(Color alpha, Color blue)
{
	return memcmp(&alpha, &blue, sizeof(Color)) == 0;
}
inline bool operator!=(Color alpha, Color blue) { return !(alpha == blue); }
wstring ToWString(Color color);
void BlendColor(Color& backgroundColor, BYTE& backgroundAlpha, Color foregroundColor, BYTE foregroundAlpha);

struct Tile
{
	int width = 0, height = 0;
	vector<Color> colors;
	vector<BYTE> alphaValues;

	Tile() = default;
	Tile(int width, int height, Color color = {}, BYTE alpha = 0);
	Tile(int width, int height, Tile const& sourceTile, DRect sourceViewport);// sourceTile 不能为空。
	Tile(wstring const& fileName, bool* isLoaded = NULL);
	void Save(wstring const& fileName) const;

	int GetArea() const;
	DRect Rect() const;
	void Fill(Color color, BYTE alpha = 255);
};
typedef vector<double> DepthBuffer;

