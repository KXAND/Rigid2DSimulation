#include "DragBar.h"
#include "App.h"
#include "DrawGeometry.h"
#include "DrawString.h"


HorizontalDragBar::HorizontalDragBar(int width) : barWidth(width) {
	gap = { 10, 10 }; depth = 1000;
	barHeight = 2; nodeWidth = 12; nodeHeight = 30;

	textColor = { 255, 255, 255 };
	barColor = { 80, 80, 80 };
	normalColor = { 80, 80, 80 };
	hoveredColor = { 20, 20, 255 };
	invalidColor = { 0, 0, 0 };
	draggedColor = { 250, 20, 20 };
}
int HorizontalDragBar::GetWidth() const { return barWidth + gap.x + textWidth; }
int HorizontalDragBar::GetHeight() const { return nodeHeight + gap.y; }

// 下面的表达式是正确的，不需要加一减一什么的。
double HorizontalDragBar::GetNormalizedValue() const { return (double)nodeX / (barWidth - nodeWidth); }
void HorizontalDragBar::SetNormalizedNodePosition(double normalizedValue) {
	int newNodeX = normalizedValue * (barWidth - nodeWidth);
	nodeX = Clamp(newNodeX, 0, barWidth - nodeWidth);
}
DVector2 HorizontalDragBar::GetNodeTopLeft() const { return topLeft + DVector2(textWidth + nodeX, 0); }
void HorizontalDragBar::Render(App& app) {
	DVector2 textTopLeft = topLeft + DVector2(0, (nodeHeight - app.font.h) / 2);
	DrawString(app.screen, app.depthBuffer, depth, text, textColor, app.font, textTopLeft, 0, viewport);

	Color const& color =
		!enabled ? invalidColor :
		dragged ? draggedColor :
		(hovered && !app.input.mouseButtonDown[0] && !app.input.mouseButtonDown[1] && !app.input.mouseButtonDown[2]) ? hoveredColor : normalColor;
	DVector2 barTopLeft = topLeft + DVector2(textWidth, (nodeHeight - barHeight) / 2);
	DrawRectangleRaw(app.screen, barTopLeft, barWidth, barHeight, viewport, barColor);
	DrawRectangleRaw(app.screen, GetNodeTopLeft(), nodeWidth, nodeHeight, viewport, color);
}

void HorizontalDragBar::Update(App& app) {
	hovered = (app.inputOwners.hovered == this);
	textWidth = GetStringSize(text, app.font, 0).x;
	if (dragged) {
		nodeX = Clamp(app.input.mousePosition.x - (topLeft.x + textWidth + nodeWidth / 2), 0, barWidth - nodeWidth);
		// 在刚刚抓住的那一帧没有更新，在放下的那一帧还在更新。省事。
		Upload(app);
		if (enabled && !app.input.mouseButtonDown[0]) { OnDone(app); }
		dragged = enabled && app.input.mouseButtonDown[0];
	} else {
		Sync(app);
		dragged = enabled && hovered && app.input.IsMouseClickedDown(0);
	} Render(app);
}
void HorizontalDragBar::Discard(App& app) {
	app.inputOwners.Remove(this); textWidth = 0;
	dragged = hovered = false;
}
void HorizontalDragBar::PreUpdate(App& app) {
	bool ok = app.inputOwners.hoveredDepth <= depth &&
		IsInside(app.input.mousePosition, { GetNodeTopLeft(), nodeWidth, nodeHeight }) && IsInside(app.input.mousePosition, viewport);
	if (ok) { app.inputOwners.hoveredDepth = depth; app.inputOwners.hovered = this; }
}

LocalizedHorizontalDragBar::LocalizedHorizontalDragBar(wstring const& id, int width) : HorizontalDragBar(width), textId(id) {}
void LocalizedHorizontalDragBar::Update(App& app) { text = app.GetLocalizedString(textId); HorizontalDragBar::Update(app); }



