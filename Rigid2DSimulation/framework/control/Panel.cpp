#include "Panel.h"
#include "App.h"
#include "Control.h"
#include "DrawBorderedRectangle.h"
#include "ScrollBar.h"


Panel::Panel() {
	depth = 1000;
	wheelMultiplier = 40;
	contentOffset = { 10, 10 };

	frameColor = { 150, 150, 150 };
	panelColor = { 0, 0, 0 };
}
// 菜单按钮的宽度: 150.
Panel Panel::CreateMenuPanel(App& app) {
	Panel panel;
	panel.w = 169;
	panel.h = app.h - 40;
	panel.visibleHeight = panel.h - 20;

	DRect contentViewportRelative;
	contentViewportRelative.topLeftPosition = DVector2(10 - 2, 10 - 2);
	contentViewportRelative.w = panel.w - 20 + 4;
	contentViewportRelative.h = panel.h - 20 + 4;
	panel.contentViewport = contentViewportRelative; return panel;
}
Panel Panel::CreateBottomPanel(App& app) {
	Panel panel;
	panel.w = app.w;
	panel.h = 40;
	panel.tl.y = app.h - panel.h;
	panel.visibleHeight = panel.h;

	DRect contentViewportRelative;
	contentViewportRelative.topLeftPosition = DVector2(10 - 2, 10 - 2);
	contentViewportRelative.w = panel.w - 20 + 4;
	contentViewportRelative.h = panel.h - 20 + 4;
	panel.contentViewport = contentViewportRelative; return panel;
}

int Panel::GetMinY() const {
	return tl.y + contentOffset.y + visibleHeight - contentControl->GetH();
}
DRect Panel::GetVisibleViewport(App& app) const {
	DRect visibleViewport = contentViewport;
	visibleViewport.topLeftPosition += tl; return GetOverlapRect(app.screen.Rect(), visibleViewport);
}
void Panel::SetControl(App& app, Control* newContentControl) {
	if (contentControl != newContentControl) {
		if (contentControl) { contentControl->Discard(app); }
		if (contentControl = newContentControl) {
			contentControl->tl = tl + contentOffset;
			contentControl->vp = GetVisibleViewport(app);
		} scrollBar->contentControl = contentControl;
	}
}
void Panel::InitializeControl(App& app) {
	if (contentControl) {
		contentControl->tl = tl + contentOffset;
		contentControl->vp = GetVisibleViewport(app);
	}
}
void Panel::InitializeScrollBar() {
	scrollBar = std::make_shared<VerticalScrollBar>();
	scrollBar->contentControl = contentControl;
	scrollBar->h = h;
	scrollBar->tl = tl + DVector2(w - scrollBar->w, 0);
	scrollBar->visibleHeight = visibleHeight;
	scrollBar->visibleTop = tl.y + contentOffset.y;
}
void Panel::Render(App& app) {
	drawRectangleWithBorderRaw(app.screen, tl, w, h, app.screen.Rect(), panelColor, frameColor);
}

void Panel::Init(App& app) { InitializeControl(app); InitializeScrollBar(); }
void Panel::Update(App& app) {
	hasOverflow = (contentControl != NULL) && visibleHeight < contentControl->GetH();
	hovered = (app.inputOwners.hovered == this);
	wheeled = (app.inputOwners.wheeled == this);

	if (hasOverflow) {
		int contentY = contentControl->tl.y;
		if (wheeled) { contentY += app.input.mouseWheelDelta * wheelMultiplier; }
		contentControl->tl.y = Clamp(contentY, GetMinY(), tl.y + contentOffset.y);
	}
	else if (contentControl) { contentControl->tl.y = tl.y + contentOffset.y; }

	// 下面 Update 顺序是为了方便绘制，本来应该控件先更新高度的。
	// 下面 Update 顺序是为了方便绘制，本来应该控件先更新高度的。
	// 这个是为了速度的牺牲，不知道有没有必要。
	Render(app);
	if (contentControl) { contentControl->Update(app); }
	if (useScrollBar) { scrollBar->Update(app); }
}
void Panel::Discard(App& app) {
	app.inputOwners.Remove(this); scrollBar->Discard(app);
	if (contentControl) { contentControl->Discard(app); }
}
void Panel::PreUpdate(App& app) {
	bool canOwnInput = app.inputOwners.hoveredDepth <= depth &&
		IsInside(app.input.mousePosition, { tl, w, h }) && IsInside(app.input.mousePosition, app.screen.Rect());
	if (canOwnInput) { app.inputOwners.hoveredDepth = depth; app.inputOwners.hovered = this; }

	canOwnInput = app.inputOwners.wheeledDepth <= depth &&
		IsInside(app.input.mousePosition, { tl, w, h }) && IsInside(app.input.mousePosition, app.screen.Rect());
	if (canOwnInput) { app.inputOwners.wheeledDepth = depth; app.inputOwners.wheeled = this; }

	if (contentControl) { contentControl->PreUpdate(app); }
	if (useScrollBar) { scrollBar->PreUpdate(app); }
}



