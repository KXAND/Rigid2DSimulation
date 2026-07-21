#include "ScrollBar.h"
#include "App.h"
#include "Control.h"
#include "DrawBorderedRectangle.h"


VerticalScrollBar::VerticalScrollBar() {
	depth = 1000; w = 20;

	barColor = { 0, 0, 0 };
	frameColor = { 150, 150, 150 };
	normalColor = { 80, 80, 80 };
	draggedColor = { 150, 60, 60 };
	hoveredColor = { 60, 60, 150 };
	invalidColor = { 0, 0, 0 };
}
void VerticalScrollBar::Render(App& app) {
	drawRectangleWithBorderRaw(app.scr, tl, w, h, app.scr.Rect(), barColor, frameColor);
	if (hasOverflow) {
		Color const& color =
			!enabled ? invalidColor :
			dragged ? draggedColor :
			(hovered && !app.input.msd[0] && !app.input.msd[1] && !app.input.msd[2]) ? hoveredColor : normalColor;
		drawRectangleWithBorderRaw(app.scr, tl + DVector2(0, thumbY), w, thumbHeight, app.scr.Rect(), color, frameColor);
	}
}

void VerticalScrollBar::Update(App& app) {
	hasOverflow = (contentControl != NULL) && visibleHeight < contentControl->GetH();
	hovered = (app.own.hovered == this);
	hoveredThumb = hovered && isInside(app.input.msp, { tl + DVector2(0, thumbY), w, thumbHeight });

	// 这个状态变化比一般的控件是要复杂的。
	if (hasOverflow) {
		thumbHeight = visibleHeight * h / contentControl->GetH();
		if (dragged) {
			thumbY += app.input.msp.y - app.input.msp_old.y;
			thumbY = clamp(thumbY, 0, h - thumbHeight);
			contentControl->tl.y = visibleTop - thumbY * contentControl->GetH() / h;
			dragged = hasOverflow && enabled && app.input.msd[0];
		} else {
			thumbY = (visibleTop - contentControl->tl.y) * h / contentControl->GetH();
			thumbY = clamp(thumbY, 0, h - thumbHeight);
			dragged = hasOverflow && enabled && hovered && app.input.IsMouseClickedDown(0);
		}
	} else { dragged = false; } Render(app);
}
void VerticalScrollBar::Discard(App& app) {
	app.own.Remove(this); dragged = hovered = hasOverflow = hoveredThumb = false;
}
void VerticalScrollBar::PreUpdate(App& app) {
	bool ok = app.own.hoveredDepth <= depth &&
		isInside(app.input.msp, { tl, w, h }) && isInside(app.input.msp, app.scr.Rect());
	if (ok) { app.own.hoveredDepth = depth; app.own.hovered = this; }
}



