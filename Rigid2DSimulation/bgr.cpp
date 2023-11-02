#include "bgr.h"
#include "cur.h"
#include "draw_str.h"
#include "draw_tile.h"

Bgr::Bgr(Cur& cur)
{
	width = 1630; height = 860;
	topLeft = { 170, 0 }; depth = -100000;
	black = tile(width, height, dColor{}, 255);
}

#include "my_def.h"

void Bgr::render(Cur& cur)
{
	draw_tile_raw(scr, topLeft, scr.rect(), black, black.rect());
	draw_str(scr, dscr, 999, dbstr,
		dColor(255), ft, topLeft + dVector2(10, 10), width - 20, bgr.viewPort());
}

void Bgr::Update(Cur& cur)
{
	hovered = (hvd == this);
	wheeled = (whd == this);

	if (hovered && msc(0))
	{
		cur.bodySelecting = NULL;
		cur.connectionSelecting = NULL;
	}
	render(cur);
}
void Bgr::PreUpdate(Cur& cur)
{
	bool haveDeeperValue = hoveredDepth <= depth && isInside(msp, viewPort());
	if (haveDeeperValue) { hoveredDepth = depth; hvd = this; }

	haveDeeperValue = whdDepth <= depth && isInside(msp, viewPort());
	if (haveDeeperValue) { whdDepth = depth; whd = this; }
}
