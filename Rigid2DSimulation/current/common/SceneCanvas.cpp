#include "SceneCanvas.h"
#include "AppItem.h"
#include "DrawTile.h"

SceneCanvas::SceneCanvas()
{
    width = 1630;
    height = 860;
    topLeft = {170, 0}; // tile的原点坐标位置，170的余量用于左侧UI显示
    depth = -100000;
    backgroundTile = Tile(width, height, Color::kBlack, 255);
}

void SceneCanvas::Render(Tile& targetTile) const
{
    DrawTileRaw(targetTile, topLeft, backgroundTile);
}

bool SceneCanvas::IsEmptyAreaClicked(AppInput& input, InputOwners const& inputOwners) const
{
    return inputOwners.hovered == this && input.IsMouseClickedDown(0);
}

void SceneCanvas::PreUpdate(AppInput const& input, InputOwners& inputOwners)
{
    bool haveDeeperValue = inputOwners.hoveredDepth <= depth && IsInside(input.mousePosition, GetViewport());
    if (haveDeeperValue)
    {
        inputOwners.hoveredDepth = depth;
        inputOwners.hovered = this;
    }

    haveDeeperValue = inputOwners.wheeledDepth <= depth && IsInside(input.mousePosition, GetViewport());
    if (haveDeeperValue)
    {
        inputOwners.wheeledDepth = depth;
        inputOwners.wheeled = this;
    }
}
