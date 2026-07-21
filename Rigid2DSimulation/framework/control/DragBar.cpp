#include "DragBar.h"
#include "App.h"
#include "DrawGeometry.h"
#include "DrawString.h"


HorizontalDragBar::HorizontalDragBar(int w) : barWidth(w) {
	gap = { 10, 10 }; depth = 1000;
	barHeight = 2; nodeWidth = 12; nodeHeight = 30;

	textColor = { 255, 255, 255 };
	barColor = { 80, 80, 80 };
	normalColor = { 80, 80, 80 };
	hoveredColor = { 20, 20, 255 };
	invalidColor = { 0, 0, 0 };
	draggedColor = { 250, 20, 20 };
}
int HorizontalDragBar::GetW() const { return barWidth + gap.x + textWidth; }
int HorizontalDragBar::GetH() const { return nodeHeight + gap.y; }

// 下面的表达式是正确的，不需要加一减一什么的。
double HorizontalDragBar::GetNormalizedValue() const { return (double)nodeX / (barWidth - nodeWidth); }
void HorizontalDragBar::SetNormalizedNodePosition(double normalizedValue) {
	int newNodeX = normalizedValue * (barWidth - nodeWidth);
	nodeX = clamp(newNodeX, 0, barWidth - nodeWidth);
}
DVector2 HorizontalDragBar::GetNodeTopLeft() const { return tl + DVector2(textWidth + nodeX, 0); }
void HorizontalDragBar::Render(App& app) {
	DVector2 textTopLeft = tl + DVector2(0, (nodeHeight - app.ft.h) / 2);
	DrawString(app.scr, app.dscr, depth, text, textColor, app.ft, textTopLeft, 0, vp);

	Color const& color =
		!enabled ? invalidColor :
		dragged ? draggedColor :
		(hovered && !app.input.msd[0] && !app.input.msd[1] && !app.input.msd[2]) ? hoveredColor : normalColor;
	DVector2 barTopLeft = tl + DVector2(textWidth, (nodeHeight - barHeight) / 2);
	drawRectangleRaw(app.scr, barTopLeft, barWidth, barHeight, vp, barColor);
	drawRectangleRaw(app.scr, GetNodeTopLeft(), nodeWidth, nodeHeight, vp, color);
}

void HorizontalDragBar::Update(App& app) {
	hovered = (app.own.hovered == this);
	textWidth = GetStringSize(text, app.ft, 0).x;
	if (dragged) {
		nodeX = clamp(app.input.msp.x - (tl.x + textWidth + nodeWidth / 2), 0, barWidth - nodeWidth);
		// 在刚刚抓住的那一帧没有更新，在放下的那一帧还在更新。省事。
		Upload(app);
		if (enabled && !app.input.msd[0]) { OnDone(app); }
		dragged = enabled && app.input.msd[0];
	} else {
		Sync(app);
		dragged = enabled && hovered && app.input.isMouseClickedDown(0);
	} Render(app);
}
void HorizontalDragBar::Discard(App& app) {
	app.own.remove(this); textWidth = 0;
	dragged = hovered = false;
}
void HorizontalDragBar::PreUpdate(App& app) {
	bool ok = app.own.hoveredDepth <= depth &&
		isInside(app.input.msp, { GetNodeTopLeft(), nodeWidth, nodeHeight }) && isInside(app.input.msp, vp);
	if (ok) { app.own.hoveredDepth = depth; app.own.hovered = this; }
}

LocalizedHorizontalDragBar::LocalizedHorizontalDragBar(wstring const& id, int w) : HorizontalDragBar(w), textId(id) {}
void LocalizedHorizontalDragBar::Update(App& app) { text = app.getLocalizedString(textId); HorizontalDragBar::Update(app); }



