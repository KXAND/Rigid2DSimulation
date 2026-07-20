#include "background.h"
#include "simulation_app.h"
#include "draw_str.h"
#include "draw_tile.h"

Background::Background(SimulationApp& cur)
{
	width = 1630; height = 860;
	topLeft = { 170, 0 }; // tile的原点坐标位置，170的余量用于左侧UI显示
	depth = -100000;
	backgroundTile = tile(width, height, dColor::BLACK, 255);
}

void Background::Render(SimulationApp& cur)
{
	drawBasedTile((cur.scr), topLeft,  backgroundTile);
	draw_str((cur.scr), (cur.dscr), 999, (cur.debugString),
		dColor(110), (cur.ft), topLeft + dVector2(10, 10), width - 20, cur.background->ViewPort());
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


