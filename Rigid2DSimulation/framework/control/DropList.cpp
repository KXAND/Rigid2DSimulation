#include "DropList.h"
#include "App.h"
#include "DrawGeometry.h"
#include "DrawString.h"
#include "DrawBorderedRectangle.h"


wstring nm_none(int id, App &app) { return L"空"; }
DropList::DropList(int w, DropListItemNameProvider const& itemNameProvider) : w(w), itemNameProvider(itemNameProvider) {
	gap = { 10, 10 }; depth = 1000;
	itemHeight = 25; h = 30;

	signRadius = 5;
	signMargin = 15;
	textMarginX = 5;

	// 懒得区分标签和内容的颜色了。
	textColor = { 255, 255, 255 };
	markColor = { 255, 255, 255 };
	editColor = { 150, 60, 60 };
	frameColor = { 150, 150, 150 };
	normalColor = { 40, 40, 40 };
	invalidColor = { 0, 0, 0 };
	hoveredColor = { 60, 60, 150 };
}
int DropList::GetW() const { return w + gap.x + textWidth; }
int DropList::GetH() const {
	return h + gap.y + (edit ? itemCount * itemHeight : 0);
}

DVector2 DropList::GetBoxTopLeft() const { return tl + DVector2(textWidth, 0); }
DVector2 DropList::GetItemsTopLeft() const { return GetBoxTopLeft() + DVector2(0, h); }
void DropList::RenderMain(App& app) {
	DVector2 labelTextTopLeft = tl + DVector2(0, (h - app.font.h) / 2);
	DrawString(app.screen, app.depthBuffer, depth, text, textColor, app.font, labelTextTopLeft, 0, vp);

	Color const& color =
		!enabled ? invalidColor :
		edit ? editColor :
		(hovered && !app.input.mouseButtonDown[0] && !app.input.mouseButtonDown[1] && !app.input.mouseButtonDown[2]) ? hoveredColor : normalColor;
	drawRectangleWithBorder(app.screen, app.depthBuffer, depth, GetBoxTopLeft(), w, h, vp, color, frameColor);

	Vector2 signCenter = (Vector2)GetBoxTopLeft() + Vector2(w - signMargin, h / 2);
	drawEllipse(app.screen, app.depthBuffer, depth, signCenter, signRadius, signRadius, vp, markColor);

	DVector2 selectedItemTextTopLeft = tl + DVector2(textMarginX, (h - app.font.h) / 2);
	DrawString(app.screen, app.depthBuffer, depth, itemNameProvider(selectedIndex, app), textColor, app.font, labelTextTopLeft, 0, vp);
}
void DropList::RenderItems(App& app) {
	if (edit) {
		drawRectangleWithBorder(app.screen, app.depthBuffer, depth,
			GetItemsTopLeft(), w, itemHeight * itemCount, vp, normalColor, frameColor);
		if (hoveredItemIndex != -1) {
			DVector2 hoveredItemTopLeft = GetBoxTopLeft() + DVector2(0, h + itemHeight * hoveredItemIndex);
			drawRectangleWithBorder(app.screen, app.depthBuffer, depth,
				hoveredItemTopLeft, w, itemHeight, vp, hoveredColor, frameColor);
		}
		for (int i = (0), endIndex = int(itemCount); i < endIndex; ++i) {
			DVector2 itemTextTopLeft = GetBoxTopLeft() +
				DVector2(0, h + itemHeight * i) +
				DVector2(textMarginX, (itemHeight - app.font.h) / 2);
			DrawString(app.screen, app.depthBuffer, depth, itemNameProvider(i, app), textColor, app.font, itemTextTopLeft, 0, vp);
		}
	}
}
void DropList::Render(App& app) {
	RenderMain(app); RenderItems(app);
}

// 在内容可变的情形下怎么处理我还没想清楚的。
void DropList::Update(App& app) {
	hovered = (app.inputOwners.hovered == this);
	textWidth = GetStringSize(text, app.font, 0).x;
	if (edit) {
		if (hovered) {
			int itemRelativeY = app.input.mousePosition.y - GetBoxTopLeft().y - h;
			hoveredItemIndex = itemRelativeY >= 0 ? itemRelativeY / itemHeight : -1;
		}
		if (enabled && hovered && app.input.IsMouseClickedDown(0)) {
			if (hoveredItemIndex != -1) { selectedIndex = hoveredItemIndex; }
			Upload(app);
		}
		if (!enabled || app.input.IsMouseClickedDown(0)) { hoveredItemIndex = -1; edit = false; }
	} else {
		Sync(app);
		edit = enabled && hovered && app.input.IsMouseClickedDown(0);
	} Render(app);
}
void DropList::Discard(App& app) {
	app.inputOwners.Remove(this); textWidth = 0;
	edit = hovered = false; hoveredItemIndex = -1;
}
void DropList::PreUpdate(App& app) {
	int visibleHeight = h + (edit ? itemCount * itemHeight : 0);
	bool ok = app.inputOwners.hoveredDepth <= depth &&
		IsInside(app.input.mousePosition, { GetBoxTopLeft(), w, visibleHeight}) && IsInside(app.input.mousePosition, vp);
	if (ok) { app.inputOwners.hoveredDepth = depth; app.inputOwners.hovered = this; }
}

LocalizedDropList::LocalizedDropList(wstring const& id, int w, DropListItemNameProvider const& itemNameProvider) : DropList(w, itemNameProvider), textId(id) {}
void LocalizedDropList::Update(App& app) { text = app.GetLocalizedString(textId); DropList::Update(app); }



