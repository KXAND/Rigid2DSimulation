#include "float_panel.h"
#include "app.h"
#include "control.h"
#include "scrollbar.h"
#include "draw_str.h"
#include "draw_geo.h"
#include "draw_comp.h"
#include "draw_px_seg.h"

#include "ctrl_def.h"

FloatPanel::FloatPanel() {
	dep = 1000;
	txt = L"窗口";
	wheel_mtp = 40;
	tl_show_rel = { 10, 40 };

	h_bar = 30;
	s_close = 20;
	half_s_mark = 5;
	margin_close = 5;
	margin_title = 5;

	c_txt = { 255, 255, 255 };
	c_mark = { 255, 255, 255 };
	c_frame = { 150, 150, 150 };
	c_panel = { 0, 0, 0 };

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
	tl = dVector2(scr.width - w, scr.height - h) / 2;
	h_show = h - 50; use_sb = true;

	dRect tmp;
	tmp.topLeftPosition = dVector2(10 - 2, 40 - 2);
	tmp.w = w - 40 + 4;
	tmp.h = h - 50 + 4;
	vp_show_rel = tmp;
}

int FloatPanel::min_y() const {
	return tl.y + tl_show_rel.y + h_show - c->GetH();
}
dRect FloatPanel::vp_show(App& app) const {
	dRect vp = vp_show_rel;
	vp.topLeftPosition += tl; return getOverlapRect(vpscr, vp);
}
dVector2 FloatPanel::tl_close() const {
	int x_rel = w - margin_close - s_close;
	int y_rel = (h_bar - s_close) / 2;
	return tl + dVector2(x_rel, y_rel);
}
void FloatPanel::set_c(App& app, Control* _c) {
	if (c != _c) {
		if (c) { c->Discard(app); }
		if (c = _c) {
			c->tl = tl + tl_show_rel;
			c->vp = vp_show(app);
		} sb->c = c;
	}
}
void FloatPanel::set_tl(App& app, dVector2 const& p) {
	if (c) {
		c->tl += p - tl;
		// 在调用 vp_show 之前需要先更新 tl。
		tl = p; c->vp = vp_show(app);
	}
	sb->tl = p + dVector2(w - sb->w, h_bar);
	sb->top_show = p.y + tl_show_rel.y;
}
void FloatPanel::init_c(App& app) {
	if (c) {
		c->tl = tl + tl_show_rel;
		c->vp = vp_show(app);
	}
}
void FloatPanel::init_sb() {
	mkp(sb)();
	sb->c = c;
	sb->h = h - h_bar;
	sb->tl = tl + dVector2(w - sb->w, h_bar);
	sb->h_show = h_show;
	sb->top_show = tl.y + tl_show_rel.y;
}

void FloatPanel::hide(App& app) { shown = false; Discard(app); }
void FloatPanel::show(App& app) { shown = true; t_click = dur; }
void FloatPanel::deal_caption(App& app) {
	hovered_close = hovered && isInside(msp, { tl_close(), s_close, s_close });
	hovered_bar = hovered && !hovered_close && isInside(msp, { tl, w, h_bar });

	if (dragged_bar) {
		dVector2 tmp = tl + msp - msp_old;
		tmp.x = clamp(tmp.x, 0, scr.width - w);
		tmp.y = clamp(tmp.y, 0, scr.height - h_bar); set_tl(app, tmp);
		dragged_bar = msd[0];
	}
	else { dragged_bar = hovered_bar && msc(0); }

	if (chosen_close) {
		if (enabled_close && !msd[0]) { hide(app); return; }
		chosen_close = enabled_close && hovered_close && msd[0];
	}
	else { chosen_close = enabled_close && hovered_close && msc(0); }
}
void FloatPanel::wheel_and_clamp_cy(App& app) {
	if (full) {
		int cy = c->tl.y;
		if (wheeled) { cy += msw * wheel_mtp; }
		c->tl.y = clamp(cy, min_y(), tl.y + tl_show_rel.y);
	}
	else if (c) { c->tl.y = tl.y + tl_show_rel.y; }
}

void FloatPanel::render_main(App& app) {
	drawRectangleWithBorderRaw(scr, tl, w, h, vpscr, c_panel, c_frame);
	dColor const& c_bar =
		dragged_bar ? c_bar_dragged :
		hovered_bar && msnd ? c_bar_hovered : c_bar_normal;
	drawRectangleWithBorderRaw(scr, tl, w, h_bar, vpscr, c_bar, c_frame);

	dVector2 tl_title = tl + dVector2(margin_title, (h_bar - ft.h) / 2);
	draw_str(scr, dscr, dep, txt, c_txt, ft, tl_title, 0, vpscr);
}
void FloatPanel::render_close(App& app) {
	dColor const& c_close =
		!enabled_close ? c_close_invalid :
		chosen_close ? c_close_chosen :
		hovered_close && msnd ? c_close_hovered : c_close_normal;
	drawRectangleRaw(scr, tl_close(), s_close, s_close, vpscr, c_close);

	dVector2 mark_ct = tl_close() + dVector2(s_close, s_close) / 2;
	dVector2 mark_arm = { half_s_mark , half_s_mark };
	drawLineSegment(scr, dscr, mark_ct - mark_arm, mark_ct + mark_arm, dep, vpscr, c_mark);
	mark_arm = { -half_s_mark , half_s_mark };
	drawLineSegment(scr, dscr, mark_ct - mark_arm, mark_ct + mark_arm, dep, vpscr, c_mark);
}
void FloatPanel::render(App& app) {
	render_main(app); render_close(app);
}

void FloatPanel::Init(App& app) { init_c(app); init_sb(); }
// 这也是比较特殊，相当于要处理自己的 Discard。
void FloatPanel::Update(App& app) {
	if (!shown) { return; }
	full = (c != NULL) && h_show < c->GetH();
	hovered = (hvd == this);
	wheeled = (whd == this);

	wheel_and_clamp_cy(app);
	if (hovered && msc(0)) { t_click = dur; }
	// 这项摆在最后是因为他可能执行 hide 指令。
	deal_caption(app);

	// 我觉得这里乱掉了。但是为了这个重构好像不值得。
	if (shown) {
		render(app);
		if (c) { c->Update(app); }
		if (use_sb) { sb->Update(app); }
	}
}
void FloatPanel::Discard(App& app) {
	chosen_close = hovered_close =
		hovered = wheeled = hovered_bar = dragged_bar = false;
	rmv; sb->Discard(app);
	if (c) { c->Discard(app); }
}
void FloatPanel::PreUpdate(App& app) {
	if (!shown) return;
	bool ok = hoveredDepth <= dep &&
		isInside(msp, { tl, w, h }) && isInside(msp, vpscr);
	if (ok) { hoveredDepth = dep; hvd = this; }

	ok = whdDepth <= dep &&
		isInside(msp, { tl, w, h }) && isInside(msp, vpscr);
	if (ok) { whdDepth = dep; whd = this; }

	if (c) { c->PreUpdate(app); }
	if (use_sb) { sb->PreUpdate(app); }
}

FPnLan::FPnLan(App& app, int w, int h, wstring const& id) :
	FloatPanel(app, w, h), id_txt(id) {}
void FPnLan::Update(App& app) {
	txt = getLocalizedString(id_txt); FloatPanel::Update(app);
}

bool less_fpn(FloatPanel* const fp0, FloatPanel* const fp1) {
	return fp0->t_click < fp1->t_click;
}
