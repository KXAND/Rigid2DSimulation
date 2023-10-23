#include "scrollbar.h"
#include "app.h"
#include "control.h"
#include "draw_comp.h"

#include "ctrl_def.h"

ScrollbarY::ScrollbarY() {
	dep = 1000; w = 20;

	c_bar = { 0, 0, 0 };
	c_frame = { 150, 150, 150 };
	c_normal = { 80, 80, 80 };
	c_dragged = { 150, 60, 60 };
	c_hovered = { 60, 60, 150 };
	c_invalid = { 0, 0, 0 };
}
void ScrollbarY::render(App& app) {
	drawRectangleWithBorderRaw(scr, tl, w, h, vpscr, c_bar, c_frame);
	if (full) {
		dColor const& c =
			!enabled ? c_invalid :
			dragged ? c_dragged :
			rhvd ? c_hovered : c_normal;
		drawRectangleWithBorderRaw(scr, tl + dVector2(0, ny_rel), w, nh, vpscr, c, c_frame);
	}
}

void ScrollbarY::Update(App& app) {
	full = (c != NULL) && h_show < c->GetH();
	hovered = (hvd == this);
	hovered_node = hovered && isInside(msp, { tl + dVector2(0, ny_rel), w, nh });

	// 这个状态变化比一般的控件是要复杂的。
	if (full) {
		nh = h_show * h / c->GetH();
		if (dragged) {
			ny_rel += msp.y - msp_old.y;
			ny_rel = clamp(ny_rel, 0, h - nh);
			c->tl.y = top_show - ny_rel * c->GetH() / h;
			dragged = full && enabled && msd[0];
		} else {
			ny_rel = (top_show - c->tl.y) * h / c->GetH();
			ny_rel = clamp(ny_rel, 0, h - nh);
			dragged = full && enabled && hovered && msc(0);
		}
	} else { dragged = false; } render(app);
}
void ScrollbarY::Discard(App& app) {
	rmv; dragged = hovered = full = hovered_node = false;
}
void ScrollbarY::PreUpdate(App& app) {
	bool ok = dhv <= dep &&
		isInside(msp, { tl, w, h }) && isInside(msp, vpscr);
	if (ok) { dhv = dep; hvd = this; }
}
