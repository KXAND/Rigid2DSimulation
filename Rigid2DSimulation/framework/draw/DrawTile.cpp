#include "DrawTile.h"

void DrawTileRaw(Tile& targetTile, DVector2 topLeft, Tile const& sourceTile)
{
	auto targetViewport = targetTile.Rect();
	auto sourceViewport = sourceTile.Rect();

	int l = max(targetViewport.GetLeft(), topLeft.x);
	int r = min(targetViewport.GetRight(), topLeft.x + sourceViewport.width);
	int t = max(targetViewport.GetTop(), topLeft.y);
	int b = min(targetViewport.GetBottom(), topLeft.y + sourceViewport.height);
	if (l >= r || t >= b) { return; } // 坐标系为左上为原点，所以正常情况是 l<r, t<b

	for (int j = (t), jEnd = int(b); j < jEnd; ++j)
	{
		int targetPixelIndex = j * targetTile.width + l;
		int sourcePixelIndex = (j - topLeft.y + sourceViewport.GetTop()) * sourceTile.width +
			(l - topLeft.x + sourceViewport.GetLeft());//将点/像素从世界坐标系转换到scrtile的内部坐标系
		int rowSize = sizeof(Color) * (r - l);
		memcpy(targetTile.colors.data() + targetPixelIndex, sourceTile.colors.data() + sourcePixelIndex, rowSize);
	}
}

void DrawTile(Tile& targetTile, DepthBuffer& depthBuffer, double depth, DVector2 topLeft,
	DRect targetViewport, Tile const& sourceTile, DRect sourceViewport)
{
	int l = max(targetViewport.GetLeft(), topLeft.x);
	int r = min(targetViewport.GetRight(), topLeft.x + sourceViewport.width);
	int t = max(targetViewport.GetTop(), topLeft.y);
	int b = min(targetViewport.GetBottom(), topLeft.y + sourceViewport.height);
	if (l >= r || t >= b) { return; }

	for (int j = (t), jEnd = int(b); j < jEnd; ++j)
	{
		int targetPixelIndex = j * targetTile.width + l;
		int sourcePixelIndex = (j - topLeft.y + sourceViewport.GetTop()) * sourceTile.width +
			(l - topLeft.x + sourceViewport.GetLeft());
		for (int i = (l), iEnd = int(r); i < iEnd; ++i)
		{
			if (depthBuffer[targetPixelIndex] <= depth && sourceTile.as[sourcePixelIndex] != 0)
			{
				depthBuffer[targetPixelIndex] = depth; targetTile.colors[targetPixelIndex] = sourceTile.colors[sourcePixelIndex];
			}
			targetPixelIndex++;
			sourcePixelIndex++;
		}
	}
}

// 疑似弃用
bool HitTestTile(DVector2 point, DVector2 topLeft, Tile const& sourceTile, DRect sourceViewport)
{
	// 假设了vpsrc和 tilesrc 左上角对齐
	DVector2 sourcePoint = sourceViewport.topLeftPosition + point - topLeft;
	int pixelIndex = sourcePoint.y * sourceTile.width + sourcePoint.x;
	return IsInside(sourcePoint, sourceViewport) && sourceTile.as[pixelIndex] != 0;
}


