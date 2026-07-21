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
	drawBasedTile((cur.scr), topLeft,  backgroundTile);
	DrawString((cur.scr), (cur.dscr), 999, (cur.debugString),
		Color(110), (cur.ft), topLeft + DVector2(10, 10), width - 20, cur.background->ViewPort());
}

void Background::Update(SimulationApp& cur)
{
	hovered = ((cur.own.hovered) == this);
	wheeled = ((cur.own.wheeled) == this);

	if (hovered && (cur.input.isMouseClickedDown)(0))
	{
		cur.bodySelecting = NULL;
		cur.connectionSelecting = NULL;
	}
}
void Background::PreUpdate(SimulationApp& cur)
{
	bool haveDeeperValue = ((cur.own).hoveredDepth) <= depth && isInside(cur.input.msp, ViewPort());
	if (haveDeeperValue) { ((cur.own).hoveredDepth) = depth; ((cur.own).hovered) = this; }

	haveDeeperValue = (cur.own.wheeledDepth) <= depth && isInside(cur.input.msp, ViewPort());
	if (haveDeeperValue) { (cur.own.wheeledDepth) = depth; (cur.own.wheeled) = this; }
}


