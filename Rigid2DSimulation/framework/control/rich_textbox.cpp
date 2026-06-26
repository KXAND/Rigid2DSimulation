#include "rich_textbox.h"
#include "app.h"
#include "draw_geo.h"
#include "draw_str.h"
#include "draw_comp.h"


RichTextbox::RichTextbox() : RichTextbox(0, 0) {}
RichTextbox::RichTextbox(int w, int h, bool newline) : w(w), h(h) {
	gap = { 10, 10 }; depth = 1000;

	c_str = { 255, 255, 255 };
	c_edit = { 80, 40, 40 };
	c_frame = { 150, 150, 150 };
	c_normal = { 40, 40, 40 };
	c_cursor = { 255, 255, 255 };
	hoveredColor = { 40, 40, 80 };
	c_invalid = { 0, 0, 0 };
	selectedColor = { 150, 150, 255 };

	edit_mg = { 5, 5 };
	show_txt_mg = { 2, 2 };
	slide_mtp = 8;
	wheel_mtp = 40;
	twinkle_mtp = 8;
	w_cursor = 2; h_cursor = 20;

	if (newline) { w_str = w - 2 * edit_mg.x; }
}
int RichTextbox::GetW() const { return w + gap.x; }
int RichTextbox::GetH() const { return h + gap.y; }

dRect RichTextbox::vp_show_txt() const {
	dVector2 tl_show_txt = tl + show_txt_mg;
	int w_show_txt = w - 2 * show_txt_mg.x;
	int h_show_txt = h - 2 * show_txt_mg.y;
	return getOverlapRect(vp, { tl_show_txt, w_show_txt, h_show_txt });
}
wstring RichTextbox::left() const {
	auto a = str.begin();
	auto b = str.begin() + edit_begin();
	return wstring(a, b);
}
wstring RichTextbox::mid() const {
	auto a = str.begin() + edit_begin();
	auto b = str.begin() + edit_end();
	return wstring(a, b);
}
wstring RichTextbox::right() const {
	auto a = str.begin() + edit_end();
	auto b = str.end();
	return wstring(a, b);
}
int RichTextbox::edit_begin() const { return min(edit_a, edit_b); }
int RichTextbox::edit_end() const { return max(edit_a, edit_b); }
bool RichTextbox::selected() const { return edit_a != edit_b; }
int RichTextbox::max_x_cs_rel() const { return w - edit_mg.x; }
int RichTextbox::max_y_cs_rel() const { return h - edit_mg.y; }
dVector2 RichTextbox::p_cs_rel(App& app) const {
	wstring tmp(str.begin(), str.begin() + edit_b);
	int x = 0; dVector2 p = str_wh(tmp, app.ft, w_str, &x);
	p.x = x; p.y -= app.ft.h;
	return (dVector2)tl_str_rel + p;
}

void RichTextbox::select_a(App& app) {
	selecting = true;
	dVector2 p = app.input.msp - (tl + (dVector2)tl_str_rel);
	edit_a = edit_b = str_index_at_pos(str, app.ft, p, w_str);
	update_x_cs_rel_hist(app);
}
void RichTextbox::select_b(App& app) {
	dVector2 p = app.input.msp - (tl + (dVector2)tl_str_rel);
	edit_b = str_index_at_pos(str, app.ft, p, w_str);
	update_x_cs_rel_hist(app);
}
void RichTextbox::update_x_cs_rel_hist(App& app) {
	auto s = wstring(str.begin(), str.begin() + edit_b);
	int tmp = 0; str_wh(s, app.ft, w_str, &tmp);
	x_cs_rel_hist = tmp;
}

void RichTextbox::add_chs(App& app) {
	wstring sifted = sift_chs(app.input.chs, false);
	if (!sifted.empty()) {
		selecting = false;
		str = left() + sifted + right();
		edit_a = edit_b = edit_begin() + sifted.size();
		update_x_cs_rel_hist(app);
	}
}
void RichTextbox::hdl_left(App& app) {
	selecting = false;
	cursor_change = true;

	if (selected()) { edit_a = edit_b = edit_begin(); }
	else {
		if (--edit_a < 0) { edit_a = 0; }
		edit_b = edit_a;
	} update_x_cs_rel_hist(app);
}
void RichTextbox::hdl_right(App& app) {
	selecting = false;
	cursor_change = true;

	if (selected()) { edit_a = edit_b = edit_end(); }
	else {
		// 下面不要减一。
		if (++edit_a > str.size()) { edit_a = str.size(); }
		edit_b = edit_a;
	} update_x_cs_rel_hist(app);
}
void RichTextbox::hdl_up(App& app) {
	selecting = false;
	cursor_change = true;

	if (selected()) { edit_a = edit_b = edit_begin(); }
	else {
		wstring tmp(str.begin(), str.begin() + edit_b);
		int y = str_wh(tmp, app.ft, w_str).y - 2 * app.ft.h;
		dVector2 p(x_cs_rel_hist, y);
		edit_a = edit_b = str_index_at_pos(str, app.ft, p, w_str);
	}
}
void RichTextbox::hdl_down(App& app) {
	selecting = false;
	cursor_change = true;

	if (selected()) { edit_a = edit_b = edit_end(); }
	else {
		wstring tmp(str.begin(), str.begin() + edit_b);
		int y = str_wh(tmp, app.ft, w_str).y;
		dVector2 p(x_cs_rel_hist, y);
		edit_a = edit_b = str_index_at_pos(str, app.ft, p, w_str);
	}
}
void RichTextbox::hdl_back(App& app) {
	selecting = false;
	cursor_change = true;

	if (selected()) {
		str = left() + right();
		edit_a = edit_b = edit_begin();
	} else if (edit_begin() > 0) {
		auto a = str.begin();
		auto b = str.begin() + edit_a - 1;
		str = wstring(a, b) + right();
		edit_b = --edit_a;
	} update_x_cs_rel_hist(app);
}
void RichTextbox::hdl_delete(App& app) {
	selecting = false;
	cursor_change = true;

	if (selected()) {
		str = left() + right();
		edit_a = edit_b = edit_begin();
	} else if (edit_end() < str.size()) {
		auto a = str.begin() + edit_a + 1;
		auto b = str.end();
		str = left() + wstring(a, b);
	}
	// 按道理 delete 不用更新的。
	// update_x_cs_rel_hist(app);
}
void RichTextbox::hdl_clip(App& app) {
	selecting = false;
	set_clipboard(mid());
	str = left() + right();
	edit_a = edit_b = edit_begin();
	update_x_cs_rel_hist(app);
}
void RichTextbox::hdl_copy(App& app) { set_clipboard(mid()); }
void RichTextbox::hdl_paste(App& app) {
	selecting = false;
	wstring sifted = sift_chs(get_clipboard(), false);
	str = left() + sifted + right();
	edit_a = edit_b = edit_begin() + sifted.size();
	update_x_cs_rel_hist(app);
}
void RichTextbox::hdl_all(App& app) {
	selecting = false;
	edit_a = 0; edit_b = str.size();
	update_x_cs_rel_hist(app);
}
void RichTextbox::hdl_key(App& app) {
	cursor_change = false;

	if (app.input.kbmsg[VK_LEFT]) { hdl_left(app); }
	else if (app.input.kbmsg[VK_RIGHT]) { hdl_right(app); }
	else if (app.input.kbmsg[VK_UP]) { hdl_up(app); }
	else if (app.input.kbmsg[VK_DOWN]) { hdl_down(app); }
	else if (app.input.kbmsg[VK_DELETE]) { hdl_delete(app); }
	else if (app.input.kbmsg[VK_BACK]) { hdl_back(app); }

	if (app.input.kbd[VK_CONTROL]) {
		if (app.input.isKeyPressedDown('C')) { hdl_copy(app); }
		else if (app.input.isKeyPressedDown('X')) { hdl_clip(app); }
		else if (app.input.isKeyPressedDown('V')) { hdl_paste(app); }
		else if (app.input.isKeyPressedDown('A')) { hdl_all(app); }
	}
}

void RichTextbox::clamp_cs_selecting(App& app) {
	double val = (app.input.msp.x - tl.x) - max_x_cs_rel();
	if (val > 0) { tl_str_rel.x -= app.fps.dt() * slide_mtp * val; }
	val = edit_mg.x - (app.input.msp.x - tl.x);
	if (val > 0) { tl_str_rel.x += app.fps.dt() * slide_mtp * val; }
	val = (app.input.msp.y - tl.y) - max_y_cs_rel();
	if (val > 0) { tl_str_rel.y -= app.fps.dt() * slide_mtp * val; }
	val = edit_mg.y - (app.input.msp.y - tl.y);
	if (val > 0) { tl_str_rel.y += app.fps.dt() * slide_mtp * val; }
}
void RichTextbox::clamp_cs_not_selecting(App& app) {
	if (p_cs_rel(app).x > max_x_cs_rel()) {
		tl_str_rel.x -= p_cs_rel(app).x - max_x_cs_rel();
	} else if (p_cs_rel(app).x < edit_mg.x) {
		tl_str_rel.x += edit_mg.x - p_cs_rel(app).x;
	}
	// 我们不对 y 这么做，主要是为了让鼠标滚轮正常，其实细想挺奇怪的。
}
void RichTextbox::clamp_cursor(App& app) {
	if (selecting) { clamp_cs_selecting(app); }
	else { clamp_cs_not_selecting(app); }
}
void RichTextbox::clamp_str(App& app) {
	// 除了文字过少以外，clamp_selecting 和 wheel 后也需要这一步。
	dVector2 p_end_rel = (dVector2)tl_str_rel + str_wh(str, app.ft, w_str);
	if (p_end_rel.x < max_x_cs_rel()) {
		tl_str_rel.x += max_x_cs_rel() - p_end_rel.x;
	}
	if (p_end_rel.y < max_y_cs_rel()) {
		tl_str_rel.y += max_y_cs_rel() - p_end_rel.y;
	}
	// 下面两行是配合上面的。
	if (tl_str_rel.x > edit_mg.x) { tl_str_rel.x = edit_mg.x; }
	if (tl_str_rel.y > edit_mg.y) { tl_str_rel.y = edit_mg.y; }
}

void RichTextbox::render_main(App& app) {
	dColor c =
		!enabled ? c_invalid :
		edit ? c_edit :
		(hovered && !app.input.msd[0] && !app.input.msd[1] && !app.input.msd[2]) ? hoveredColor : c_normal;
	drawRectangleWithBorderRaw(app.scr, tl, w, h, vp, c, c_frame);

	int x_cur = 0;
	dVector2 tl_txt = tl + (dVector2)tl_str_rel;
	draw_str(app.scr, app.dscr, depth, left(), c_str, app.ft, tl_txt, x_cur, w_str, vp_show_txt());
	draw_str(app.scr, app.dscr, depth, mid(), selectedColor, app.ft, tl_txt, x_cur, w_str, vp_show_txt());
	draw_str(app.scr, app.dscr, depth, right(), c_str, app.ft, tl_txt, x_cur, w_str, vp_show_txt());
}
void RichTextbox::render_cursor(App& app) {
	bool show_cursor = edit &&
		(sin(app.fps.dur() * twinkle_mtp) > 0 || cursor_change);
	if (show_cursor) {
		dVector2 tl_cs = tl + p_cs_rel(app) + dVector2(-w_cursor, app.ft.h - h_cursor) / 2;
		drawRectangleRaw(app.scr, tl_cs, w_cursor, h_cursor, vp_show_txt(), c_cursor);
	}
}
void RichTextbox::render(App& app) {
	render_main(app); render_cursor(app);
}

void RichTextbox::Update(App& app) {
	hovered = (app.own.hovered == this);
	if (edit) {
		// 我感觉 selecting 有关的逻辑也是有点乱的。
		// 我感觉 selecting 有关的逻辑也是有点乱的。
		// 怎么看待 UI 交互真的是个大问题。怎么清晰的表达需求。
		if (selecting) { select_b(app); }
		if (enabled && hovered && app.input.isMouseClickedDown(0)) { select_a(app); }
		if (!app.input.msd[0]) { selecting = false; }
		hdl_key(app); add_chs(app); clamp_cursor(app); Upload(app);

		if (enabled && !hovered && app.input.isMouseClickedDown(0)) { OnDone(app); }
		if (!enabled || !hovered && app.input.isMouseClickedDown(0)) {
			// 请注意 kb_owner 的登记在 PreUpdate 中进行。
			app.own.free_kb(this); edit = selecting = false;
			edit_a = edit_b = 0; x_cs_rel_hist = 0;
		}
	} else {
		Sync(app);
		if (enabled && hovered && app.input.isMouseClickedDown(0)) {
			if (clr_upon_edit) { str.clear(); }
			select_a(app); edit = true;
		}
	} 

	if (app.own.wheeled == this) { tl_str_rel.y += app.input.msw * wheel_mtp; }
	clamp_str(app); render(app);
}
void RichTextbox::Discard(App& app) {
	app.own.remove(this); app.own.free_kb(this);
	edit = hovered = false; selecting = false;
	edit_a = edit_b = 0; x_cs_rel_hist = 0;
}
void RichTextbox::PreUpdate(App& app) {
	if (edit) { app.own.keyboardOwner = this; }

	bool ok = app.own.hoveredDepth <= depth &&
		isInside(app.input.msp, { tl, w, h }) && isInside(app.input.msp, vp);
	if (ok) { app.own.hoveredDepth = depth; app.own.hovered = this; }

	ok = app.own.wheeledDepth <= depth &&
		isInside(app.input.msp, { tl, w, h }) && isInside(app.input.msp, vp);
	if (ok) { app.own.wheeledDepth = depth; app.own.wheeled = this; }
}



