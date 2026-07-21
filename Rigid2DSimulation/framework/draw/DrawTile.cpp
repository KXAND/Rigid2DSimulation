#include "DrawTile.h"

void DrawTileRaw(Tile& targetTile, DVector2 topLeft, Tile const& sourceTile)
{
	auto targetViewport = targetTile.Rect();
	auto sourceViewport = sourceTile.Rect();

	int left = max(targetViewport.GetLeft(), topLeft.x);
	int right = min(targetViewport.GetRight(), topLeft.x + sourceViewport.width);
	int top = max(targetViewport.GetTop(), topLeft.y);
	int bottom = min(targetViewport.GetBottom(), topLeft.y + sourceViewport.height);
	if (left >= right || top >= bottom) { return; } // 坐标系为左上为原点，所以正常情况是 left<right, top<bottom

	for (int j = (top), jEnd = int(bottom); j < jEnd; ++j)
	{
		int targetPixelIndex = j * targetTile.width + left;
		int sourcePixelIndex = (j - topLeft.y + sourceViewport.GetTop()) * sourceTile.width +
			(left - topLeft.x + sourceViewport.GetLeft());//将点/像素从世界坐标系转换到scrtile的内部坐标系
		int rowSize = sizeof(Color) * (right - left);
		memcpy(targetTile.colors.data() + targetPixelIndex, sourceTile.colors.data() + sourcePixelIndex, rowSize);
	}
}

void DrawTile(Tile& targetTile, DepthBuffer& depthBuffer, double depth, DVector2 topLeft,
	DRect targetViewport, Tile const& sourceTile, DRect sourceViewport)
{
	int left = max(targetViewport.GetLeft(), topLeft.x);
	int right = min(targetViewport.GetRight(), topLeft.x + sourceViewport.width);
	int top = max(targetViewport.GetTop(), topLeft.y);
	int bottom = min(targetViewport.GetBottom(), topLeft.y + sourceViewport.height);
	if (left >= right || top >= bottom) { return; }

	for (int j = (top), jEnd = int(bottom); j < jEnd; ++j)
	{
		int targetPixelIndex = j * targetTile.width + left;
		int sourcePixelIndex = (j - topLeft.y + sourceViewport.GetTop()) * sourceTile.width +
			(left - topLeft.x + sourceViewport.GetLeft());
		for (int i = (left), iEnd = int(right); i < iEnd; ++i)
		{
			if (depthBuffer[targetPixelIndex] <= depth && sourceTile.alphaValues[sourcePixelIndex] != 0)
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
	return IsInside(sourcePoint, sourceViewport) && sourceTile.alphaValues[pixelIndex] != 0;
}


