#include "ScrollBar.h"
#include "App.h"
#include "Control.h"
#include "DrawBorderedRectangle.h"


VerticalScrollBar::VerticalScrollBar() {
	depth = 1000; w = 20;

	c_bar = { 0, 0, 0 };
	c_frame = { 150, 150, 150 };
	c_normal = { 80, 80, 80 };
	draggedColor = { 150, 60, 60 };
	hoveredColor = { 60, 60, 150 };
	c_invalid = { 0, 0, 0 };
}
void VerticalScrollBar::render(App& app) {
	drawRectangleWithBorderRaw(app.scr, tl, w, h, app.scr.Rect(), c_bar, c_frame);
	if (full) {
		Color const& c =
			!enabled ? c_invalid :
			dragged ? draggedColor :
			(hovered && !app.input.msd[0] && !app.input.msd[1] && !app.input.msd[2]) ? hoveredColor : c_normal;
		drawRectangleWithBorderRaw(app.scr, tl + DVector2(0, ny_rel), w, nh, app.scr.Rect(), c, c_frame);
	}
}

void VerticalScrollBar::Update(App& app) {
	full = (c != NULL) && h_show < c->GetH();
	hovered = (app.own.hovered == this);
	hovered_node = hovered && isInside(app.input.msp, { tl + DVector2(0, ny_rel), w, nh });

	// 这个状态变化比一般的控件是要复杂的。
	if (full) {
		nh = h_show * h / c->GetH();
		if (dragged) {
			ny_rel += app.input.msp.y - app.input.msp_old.y;
			ny_rel = clamp(ny_rel, 0, h - nh);
			c->tl.y = top_show - ny_rel * c->GetH() / h;
			dragged = full && enabled && app.input.msd[0];
		} else {
			ny_rel = (top_show - c->tl.y) * h / c->GetH();
			ny_rel = clamp(ny_rel, 0, h - nh);
			dragged = full && enabled && hovered && app.input.isMouseClickedDown(0);
		}
	} else { dragged = false; } render(app);
}
void VerticalScrollBar::Discard(App& app) {
	app.own.remove(this); dragged = hovered = full = hovered_node = false;
}
void VerticalScrollBar::PreUpdate(App& app) {
	bool ok = app.own.hoveredDepth <= depth &&
		isInside(app.input.msp, { tl, w, h }) && isInside(app.input.msp, app.scr.Rect());
	if (ok) { app.own.hoveredDepth = depth; app.own.hovered = this; }
}



