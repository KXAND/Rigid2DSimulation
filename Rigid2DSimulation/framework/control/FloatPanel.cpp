#include "FloatPanel.h"
#include "App.h"
#include "Control.h"
#include "ScrollBar.h"
#include "DrawString.h"
#include "DrawGeometry.h"
#include "DrawBorderedRectangle.h"
#include "DrawLineSegment.h"


FloatPanel::FloatPanel() {
	depth = 1000;
	txt = L"窗口";
	wheelMultiplier = 40;
	contentOffset = { 10, 40 };

	h_bar = 30;
	s_close = 20;
	half_s_mark = 5;
	margin_close = 5;
	margin_title = 5;

	c_txt = { 255, 255, 255 };
	c_mark = { 255, 255, 255 };
	frameColor = { 150, 150, 150 };
	panelColor = { 0, 0, 0 };

	c_bar_normal = { 40, 40, 40 };
	c_bar_dragged = { 80, 40, 40 };
	c_bar_hovered = { 40, 40, 80 };

	c_close_normal = { 0, 0, 0 };
	c_close_chosen = { 255, 20, 20 };
	c_close_hovered = { 20, 20, 255 };
	c_close_invalid = { 255, 255, 255 };
}
FloatPanel::FloatPanel(App& app, int _w, int _h) : FloatPanel() {
	w = _w; h = _h;
	tl = DVector2(app.scr.width - w, app.scr.height - h) / 2;
	visibleHeight = h - 50; useScrollBar = true;

	DRect tmp;
	tmp.topLeftPosition = DVector2(10 - 2, 40 - 2);
	tmp.w = w - 40 + 4;
	tmp.h = h - 50 + 4;
	contentViewport = tmp;
}

int FloatPanel::GetMinY() const {
	return tl.y + contentOffset.y + visibleHeight - contentControl->GetH();
}
DRect FloatPanel::GetVisibleViewport(App& app) const {
	DRect vp = contentViewport;
	vp.topLeftPosition += tl; return getOverlapRect(app.scr.Rect(), vp);
}
DVector2 FloatPanel::GetCloseTopLeft() const {
	int x_rel = w - margin_close - s_close;
	int y_rel = (h_bar - s_close) / 2;
	return tl + DVector2(x_rel, y_rel);
}
void FloatPanel::SetControl(App& app, Control* _c) {
	if (contentControl != _c) {
		if (contentControl) { contentControl->Discard(app); }
		if (contentControl = _c) {
			contentControl->tl = tl + contentOffset;
			contentControl->vp = GetVisibleViewport(app);
		} scrollBar->contentControl = contentControl;
	}
}
void FloatPanel::SetTopLeft(App& app, DVector2 const& p) {
	if (contentControl) {
		contentControl->tl += p - tl;
		// 在调用 GetVisibleViewport 之前需要先更新 tl。
		tl = p; contentControl->vp = GetVisibleViewport(app);
	}
	scrollBar->tl = p + DVector2(w - scrollBar->w, h_bar);
	scrollBar->visibleTop = p.y + contentOffset.y;
}
void FloatPanel::InitializeControl(App& app) {
	if (contentControl) {
		contentControl->tl = tl + contentOffset;
		contentControl->vp = GetVisibleViewport(app);
	}
}
void FloatPanel::InitializeScrollBar() {
	scrollBar = std::make_shared<VerticalScrollBar>();
	scrollBar->contentControl = contentControl;
	scrollBar->h = h - h_bar;
	scrollBar->tl = tl + DVector2(w - scrollBar->w, h_bar);
	scrollBar->visibleHeight = visibleHeight;
	scrollBar->visibleTop = tl.y + contentOffset.y;
}

void FloatPanel::Hide(App& app) { shown = false; Discard(app); }
void FloatPanel::Show(App& app) { shown = true; t_click = app.fps.dur(); }
void FloatPanel::HandleCaption(App& app) {
	hovered_close = hovered && isInside(app.input.msp, { GetCloseTopLeft(), s_close, s_close });
	hovered_bar = hovered && !hovered_close && isInside(app.input.msp, { tl, w, h_bar });

	if (dragged_bar) {
		DVector2 tmp = tl + app.input.msp - app.input.msp_old;
		tmp.x = clamp(tmp.x, 0, app.scr.width - w);
		tmp.y = clamp(tmp.y, 0, app.scr.height - h_bar); SetTopLeft(app, tmp);
		dragged_bar = app.input.msd[0];
	}
	else { dragged_bar = hovered_bar && app.input.isMouseClickedDown(0); }

	if (chosen_close) {
		if (enabled_close && !app.input.msd[0]) { Hide(app); return; }
		chosen_close = enabled_close && hovered_close && app.input.msd[0];
	}
	else { chosen_close = enabled_close && hovered_close && app.input.isMouseClickedDown(0); }
}
void FloatPanel::HandleWheelAndClampContent(App& app) {
	if (hasOverflow) {
		int cy = contentControl->tl.y;
		if (wheeled) { cy += app.input.msw * wheelMultiplier; }
		contentControl->tl.y = clamp(cy, GetMinY(), tl.y + contentOffset.y);
	}
	else if (contentControl) { contentControl->tl.y = tl.y + contentOffset.y; }
}

void FloatPanel::RenderMain(App& app) {
	drawRectangleWithBorderRaw(app.scr, tl, w, h, app.scr.Rect(), panelColor, frameColor);
	Color const& c_bar =
		dragged_bar ? c_bar_dragged :
		hovered_bar && (!app.input.msd[0] && !app.input.msd[1] && !app.input.msd[2]) ? c_bar_hovered : c_bar_normal;
	drawRectangleWithBorderRaw(app.scr, tl, w, h_bar, app.scr.Rect(), c_bar, frameColor);

	DVector2 tl_title = tl + DVector2(margin_title, (h_bar - app.ft.h) / 2);
	DrawString(app.scr, app.dscr, depth, txt, c_txt, app.ft, tl_title, 0, app.scr.Rect());
}
void FloatPanel::RenderClose(App& app) {
	Color const& c_close =
		!enabled_close ? c_close_invalid :
		chosen_close ? c_close_chosen :
		hovered_close && (!app.input.msd[0] && !app.input.msd[1] && !app.input.msd[2]) ? c_close_hovered : c_close_normal;
	drawRectangleRaw(app.scr, GetCloseTopLeft(), s_close, s_close, app.scr.Rect(), c_close);

	DVector2 mark_ct = GetCloseTopLeft() + DVector2(s_close, s_close) / 2;
	DVector2 mark_arm = { half_s_mark , half_s_mark };
	drawLineSegment(app.scr, app.dscr, mark_ct - mark_arm, mark_ct + mark_arm, depth, app.scr.Rect(), c_mark);
	mark_arm = { -half_s_mark , half_s_mark };
	drawLineSegment(app.scr, app.dscr, mark_ct - mark_arm, mark_ct + mark_arm, depth, app.scr.Rect(), c_mark);
}
void FloatPanel::Render(App& app) {
	RenderMain(app); RenderClose(app);
}

void FloatPanel::Init(App& app) { InitializeControl(app); InitializeScrollBar(); }
// 这也是比较特殊，相当于要处理自己的 Discard。
void FloatPanel::Update(App& app) {
	if (!shown) { return; }
	hasOverflow = (contentControl != NULL) && visibleHeight < contentControl->GetH();
	hovered = (app.own.hovered == this);
	wheeled = (app.own.wheeled == this);

	HandleWheelAndClampContent(app);
	if (hovered && app.input.isMouseClickedDown(0)) { t_click = app.fps.dur(); }
	// 这项摆在最后是因为他可能执行 hide 指令。
	HandleCaption(app);

	// 我觉得这里乱掉了。但是为了这个重构好像不值得。
	if (shown) {
		Render(app);
		if (contentControl) { contentControl->Update(app); }
		if (useScrollBar) { scrollBar->Update(app); }
	}
}
void FloatPanel::Discard(App& app) {
	chosen_close = hovered_close =
		hovered = wheeled = hovered_bar = dragged_bar = false;
	app.own.remove(this); scrollBar->Discard(app);
	if (contentControl) { contentControl->Discard(app); }
}
void FloatPanel::PreUpdate(App& app) {
	if (!shown) return;
	bool ok = app.own.hoveredDepth <= depth &&
		isInside(app.input.msp, { tl, w, h }) && isInside(app.input.msp, app.scr.Rect());
	if (ok) { app.own.hoveredDepth = depth; app.own.hovered = this; }

	ok = app.own.wheeledDepth <= depth &&
		isInside(app.input.msp, { tl, w, h }) && isInside(app.input.msp, app.scr.Rect());
	if (ok) { app.own.wheeledDepth = depth; app.own.wheeled = this; }

	if (contentControl) { contentControl->PreUpdate(app); }
	if (useScrollBar) { scrollBar->PreUpdate(app); }
}

LocalizedFloatPanel::LocalizedFloatPanel(App& app, int w, int h, wstring const& id) :
	FloatPanel(app, w, h), id_txt(id) {}
void LocalizedFloatPanel::Update(App& app) {
	txt = app.getLocalizedString(id_txt); FloatPanel::Update(app);
}

bool CompareFloatPanels(FloatPanel* const fp0, FloatPanel* const fp1) {
	return fp0->t_click < fp1->t_click;
}



