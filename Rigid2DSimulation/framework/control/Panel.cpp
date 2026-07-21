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
	Panel pn;
	pn.w = 169;
	pn.h = app.h - 40;
	pn.visibleHeight = pn.h - 20;

	DRect tmp;
	tmp.topLeftPosition = DVector2(10 - 2, 10 - 2);
	tmp.w = pn.w - 20 + 4;
	tmp.h = pn.h - 20 + 4;
	pn.contentViewport = tmp; return pn;
}
Panel Panel::CreateBottomPanel(App& app) {
	Panel pn;
	pn.w = app.w;
	pn.h = 40;
	pn.tl.y = app.h - pn.h;
	pn.visibleHeight = pn.h;

	DRect tmp;
	tmp.topLeftPosition = DVector2(10 - 2, 10 - 2);
	tmp.w = pn.w - 20 + 4;
	tmp.h = pn.h - 20 + 4;
	pn.contentViewport = tmp; return pn;
}

int Panel::GetMinY() const {
	return tl.y + contentOffset.y + visibleHeight - contentControl->GetH();
}
DRect Panel::GetVisibleViewport(App& app) const {
	DRect vp = contentViewport;
	vp.topLeftPosition += tl; return getOverlapRect(app.scr.Rect(), vp);
}
void Panel::SetControl(App& app, Control* _c) {
	if (contentControl != _c) {
		if (contentControl) { contentControl->Discard(app); }
		if (contentControl = _c) {
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
	drawRectangleWithBorderRaw(app.scr, tl, w, h, app.scr.Rect(), panelColor, frameColor);
}

void Panel::Init(App& app) { InitializeControl(app); InitializeScrollBar(); }
void Panel::Update(App& app) {
	hasOverflow = (contentControl != NULL) && visibleHeight < contentControl->GetH();
	hovered = (app.own.hovered == this);
	wheeled = (app.own.wheeled == this);

	if (hasOverflow) {
		int cy = contentControl->tl.y;
		if (wheeled) { cy += app.input.msw * wheelMultiplier; }
		contentControl->tl.y = clamp(cy, GetMinY(), tl.y + contentOffset.y);
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
	app.own.remove(this); scrollBar->Discard(app);
	if (contentControl) { contentControl->Discard(app); }
}
void Panel::PreUpdate(App& app) {
	bool ok = app.own.hoveredDepth <= depth &&
		isInside(app.input.msp, { tl, w, h }) && isInside(app.input.msp, app.scr.Rect());
	if (ok) { app.own.hoveredDepth = depth; app.own.hovered = this; }

	ok = app.own.wheeledDepth <= depth &&
		isInside(app.input.msp, { tl, w, h }) && isInside(app.input.msp, app.scr.Rect());
	if (ok) { app.own.wheeledDepth = depth; app.own.wheeled = this; }

	if (contentControl) { contentControl->PreUpdate(app); }
	if (useScrollBar) { scrollBar->PreUpdate(app); }
}



