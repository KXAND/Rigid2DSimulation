#include "Tile.h"

const Color Color::RED = Color(255, 0, 0);
const Color Color::GREEN = Color(0, 255, 0);
const Color Color::BLUE = Color(0, 0, 255);
const Color Color::CYAN = Color(0, 255, 255);
const Color Color::MAGENTA = Color(255, 0, 255);
const Color Color::YELLOW = Color(255, 255, 0);
const Color Color::WHITE = Color(255, 255, 255);
const Color Color::BLACK = Color(0, 0, 0);

Color::Color(wstring const& text)
{
	int position = 0;
	red = FetchNumber(position, text);
	green = FetchNumber(position, text);
	blue = FetchNumber(position, text);
}
wstring ToWString(Color color)
{
	return L"(" + std::to_wstring(color.red) + L"," + std::to_wstring(color.green) + L"," + std::to_wstring(color.blue) + L")";
}
void BlendColor(Color& backgroundColor, BYTE& backgroundAlpha, Color foregroundColor, BYTE foregroundAlpha)
{
	// 目前还不会推导公式，应该有简化的空间。
	if (foregroundAlpha == 0) { return; }
	if (foregroundAlpha == 255) { backgroundColor = foregroundColor; backgroundAlpha = 255; return; }

	int alpha = 255 * (backgroundAlpha + foregroundAlpha) - backgroundAlpha * foregroundAlpha;
	int red = (backgroundColor.red * backgroundAlpha * (255 - foregroundAlpha) + foregroundColor.red * foregroundAlpha * 255) / alpha;
	int green = (backgroundColor.green * backgroundAlpha * (255 - foregroundAlpha) + foregroundColor.green * foregroundAlpha * 255) / alpha;
	int blue = (backgroundColor.blue * backgroundAlpha * (255 - foregroundAlpha) + foregroundColor.blue * foregroundAlpha * 255) / alpha; alpha /= 255;
	backgroundColor = Color(red, green, blue); backgroundAlpha = alpha;
}

Tile::Tile(int width, int height, Color color, BYTE alpha) :
	width(width), height(height), colors(GetArea(), color), alphaValues(GetArea(), alpha)
{
}
Tile::Tile(int width, int height, Tile const& sourceTile, DRect sourceViewport) : Tile(width, height)
{
	for (int i = (0), iEnd = int(width); i < iEnd; ++i) for (int j = (0), jEnd = int(height); j < jEnd; ++j)
	{
		DVector2 targetPoint(i, j);
		DVector2 sourcePoint = targetPoint * DVector2(sourceViewport.width, sourceViewport.height) / DVector2(width, height);

		int targetPixelIndex = targetPoint.y * width + targetPoint.x;
		int sourcePixelIndex = (sourcePoint.y + sourceViewport.GetTop()) * sourceTile.width + sourcePoint.x + sourceViewport.GetLeft();
		colors[targetPixelIndex] = sourceTile.colors[sourcePixelIndex]; alphaValues[targetPixelIndex] = sourceTile.alphaValues[sourcePixelIndex];
	}
}
void Tile::Save(wstring const& fileName) const
{
	FILE* file = OpenFile(fileName, L"wb");
	if (!file) { return; }
	fwrite(&(width), sizeof(width), 1, file);
	fwrite(&(height), sizeof(height), 1, file);
	fwrite(&colors[0], sizeof(*(&colors[0])), GetArea(), file);
	fwrite(&alphaValues[0], sizeof(*(&alphaValues[0])), GetArea(), file);
	fclose(file);
}
Tile::Tile(wstring const& fileName, bool* isLoaded)
{
	FILE* file = OpenFile(fileName, L"rb");
	if (isLoaded) { *isLoaded = file; }
	if (!file) { return; }
	fread(&(width), sizeof(width), 1, file);
	fread(&(height), sizeof(height), 1, file);
	colors.resize(width * height); alphaValues.resize(width * height);
	fread(&colors[0], sizeof(*(&colors[0])), GetArea(), file);
	fread(&alphaValues[0], sizeof(*(&alphaValues[0])), GetArea(), file);
	fclose(file);
}

int Tile::GetArea() const { return width * height; }
DRect Tile::Rect() const { return { width, height }; }
void Tile::Fill(Color color, BYTE alpha)
{
	for (int i = (0), iEnd = int(GetArea()); i < iEnd; ++i) { colors[i] = color; }
	memset(alphaValues.data(), alpha, GetArea());
}

