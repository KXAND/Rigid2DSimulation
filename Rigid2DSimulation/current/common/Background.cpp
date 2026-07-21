#include "Background.h"
#include "SimulationApp.h"
#include "DrawString.h"
#include "DrawTile.h"

Background::Background(SimulationApp& cur)
{
	width = 1630; height = 860;
	topLeft = { 170, 0 }; // tile的原点坐标位置，170的余量用于左侧UI显示
	depth = -100000;
	backgroundTile = Tile(width, height, Color::BLACK, 255);
}

void Background::Render(SimulationApp& cur)
{
	drawBasedTile((cur.screen), topLeft,  backgroundTile);
	DrawString((cur.screen), (cur.depthBuffer), 999, (cur.debugString),
		Color(110), (cur.font), topLeft + DVector2(10, 10), width - 20, cur.background->ViewPort());
}

void Background::Update(SimulationApp& cur)
{
	hovered = ((cur.inputOwners.hovered) == this);
	wheeled = ((cur.inputOwners.wheeled) == this);

	if (hovered && (cur.input.IsMouseClickedDown)(0))
	{
		cur.bodySelecting = NULL;
		cur.connectionSelecting = NULL;
	}
}
void Background::PreUpdate(SimulationApp& cur)
{
	bool haveDeeperValue = ((cur.inputOwners).hoveredDepth) <= depth && isInside(cur.input.mousePosition, ViewPort());
	if (haveDeeperValue) { ((cur.inputOwners).hoveredDepth) = depth; ((cur.inputOwners).hovered) = this; }

	haveDeeperValue = (cur.inputOwners.wheeledDepth) <= depth && isInside(cur.input.mousePosition, ViewPort());
	if (haveDeeperValue) { (cur.inputOwners.wheeledDepth) = depth; (cur.inputOwners.wheeled) = this; }
}


