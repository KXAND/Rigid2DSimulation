#include "Panel.h"
#include "App.h"
#include "Control.h"
#include "DrawBorderedRectangle.h"
#include "ScrollBar.h"


Panel::Panel() {
	depth = 1000;
	wheel_mtp = 40;
	tl_show_rel = { 10, 10 };

	c_frame = { 150, 150, 150 };
	c_panel = { 0, 0, 0 };
}
// 菜单按钮的宽度: 150.
Panel Panel::menu(App& app) {
	Panel pn;
	pn.w = 169;
	pn.h = app.h - 40;
	pn.h_show = pn.h - 20;

	DRect tmp;
	tmp.topLeftPosition = DVector2(10 - 2, 10 - 2);
	tmp.w = pn.w - 20 + 4;
	tmp.h = pn.h - 20 + 4;
	pn.vp_show_rel = tmp; return pn;
}
Panel Panel::bottom(App& app) {
	Panel pn;
	pn.w = app.w;
	pn.h = 40;
	pn.tl.y = app.h - pn.h;
	pn.h_show = pn.h;

	DRect tmp;
	tmp.topLeftPosition = DVector2(10 - 2, 10 - 2);
	tmp.w = pn.w - 20 + 4;
	tmp.h = pn.h - 20 + 4;
	pn.vp_show_rel = tmp; return pn;
}

int Panel::GetMinY() const {
	return tl.y + tl_show_rel.y + h_show - c->GetH();
}
DRect Panel::GetVisibleViewport(App& app) const {
	DRect vp = vp_show_rel;
	vp.topLeftPosition += tl; return getOverlapRect(app.scr.Rect(), vp);
}
void Panel::SetControl(App& app, Control* _c) {
	if (c != _c) {
		if (c) { c->Discard(app); }
		if (c = _c) {
			c->tl = tl + tl_show_rel;
			c->vp = GetVisibleViewport(app);
		} sb->c = c;
	}
}
void Panel::InitializeControl(App& app) {
	if (c) {
		c->tl = tl + tl_show_rel;
		c->vp = GetVisibleViewport(app);
	}
}
void Panel::InitializeScrollBar() {
	sb = std::make_shared<VerticalScrollBar>();
	sb->c = c;
	sb->h = h;
	sb->tl = tl + DVector2(w - sb->w, 0);
	sb->h_show = h_show;
	sb->top_show = tl.y + tl_show_rel.y;
}
void Panel::Render(App& app) {
	drawRectangleWithBorderRaw(app.scr, tl, w, h, app.scr.Rect(), c_panel, c_frame);
}

void Panel::Init(App& app) { InitializeControl(app); InitializeScrollBar(); }
void Panel::Update(App& app) {
	full = (c != NULL) && h_show < c->GetH();
	hovered = (app.own.hovered == this);
	wheeled = (app.own.wheeled == this);

	if (full) {
		int cy = c->tl.y;
		if (wheeled) { cy += app.input.msw * wheel_mtp; }
		c->tl.y = clamp(cy, GetMinY(), tl.y + tl_show_rel.y);
	}
	else if (c) { c->tl.y = tl.y + tl_show_rel.y; }

	// 下面 Update 顺序是为了方便绘制，本来应该控件先更新高度的。
	// 下面 Update 顺序是为了方便绘制，本来应该控件先更新高度的。
	// 这个是为了速度的牺牲，不知道有没有必要。
	Render(app);
	if (c) { c->Update(app); }
	if (use_sb) { sb->Update(app); }
}
void Panel::Discard(App& app) {
	app.own.remove(this); sb->Discard(app);
	if (c) { c->Discard(app); }
}
void Panel::PreUpdate(App& app) {
	bool ok = app.own.hoveredDepth <= depth &&
		isInside(app.input.msp, { tl, w, h }) && isInside(app.input.msp, app.scr.Rect());
	if (ok) { app.own.hoveredDepth = depth; app.own.hovered = this; }

	ok = app.own.wheeledDepth <= depth &&
		isInside(app.input.msp, { tl, w, h }) && isInside(app.input.msp, app.scr.Rect());
	if (ok) { app.own.wheeledDepth = depth; app.own.wheeled = this; }

	if (c) { c->PreUpdate(app); }
	if (use_sb) { sb->PreUpdate(app); }
}



