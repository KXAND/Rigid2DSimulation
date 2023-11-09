#include "bgr.h"
#include "cur.h"
#include "draw_str.h"
#include "draw_tile.h"

Background::Background(Cur& cur)
{
	width = 1630; height = 860;
	topLeft = { 170, 0 }; // tile的原点坐标位置，170的余量用于左侧UI显示
	depth = -100000;
	backgroundTile = tile(width, height, dColor::WHITE, 255);
}

void Background::Render(Cur& cur)
{
	drawTileRaw((cur.scr), topLeft,  backgroundTile);
	draw_str((cur.scr), (cur.dscr), 999, (cur.dbstr),
		dColor(110), (cur.ft), topLeft + dVector2(10, 10), width - 20, (*cur.mBgr).viewPort());
}

void Background::Update(Cur& cur)
{
	hovered = (((cur.own).hvd) == this);
	wheeled = (((cur.own).whd) == this);

	if (hovered && (cur.input.isMouseClickedDown)(0))
	{
		cur.bodySelecting = NULL;
		cur.connectionSelecting = NULL;
	}
}
void Background::PreUpdate(Cur& cur)
{
	bool haveDeeperValue = ((cur.own).hoveredDepth) <= depth && isInside(cur.input.msp, viewPort());
	if (haveDeeperValue) { ((cur.own).hoveredDepth) = depth; ((cur.own).hvd) = this; }

	haveDeeperValue = (cur.own.whdDepth) <= depth && isInside((cur.input.msp), viewPort());
	if (haveDeeperValue) { (cur.own.whdDepth) = depth; (cur.own.whd) = this; }
}
