#include "TextBox.h"
#include "App.h"
#include "DrawGeometry.h"
#include "DrawString.h"
#include "DrawBorderedRectangle.h"
#include "RichTextBox.h"


TextBox::TextBox(int w) : w(w) {
	gap = { 10, 10 }; 
	depth = 1000; h = 30;

	c_txt = { 255, 255, 255 };
	c_str = { 255, 255, 255 };
	c_edit = { 150, 60, 60 };
	c_frame = { 150, 150, 150 };
	c_normal = { 40, 40, 40 };
	c_cursor = { 255, 255, 255 };
	hoveredColor = { 60, 60, 150 };
	c_invalid = { 0, 0, 0 };
	selectedColor = { 150, 150, 255 };

	w_edit_mg = 5;
	w_show_txt_mg = 2;
	slide_mtp = 8;
	twinkle_mtp = 8;
	w_cursor = 2; h_cursor = 20;
}
int TextBox::GetW() const { return w + gap.x + w_txt; }
int TextBox::GetH() const { return h + gap.y; }

DVector2 TextBox::tl_box() const { return tl + DVector2(w_txt, 0); }
DRect TextBox::vp_show_txt() const {
	DVector2 tl_show_txt = tl_box() + DVector2(w_show_txt_mg, 0);
	int w_show_txt = w - 2 * w_show_txt_mg;
	return getOverlapRect(vp, { tl_show_txt, w_show_txt, h });
}
wstring TextBox::left() const {
	auto a = str.begin();
	auto b = str.begin() + edit_begin();
	return wstring(a, b);
}
wstring TextBox::mid() const {
	auto a = str.begin() + edit_begin();
	auto b = str.begin() + edit_end();
	return wstring(a, b);
}
wstring TextBox::right() const {
	auto a = str.begin() + edit_end();
	auto b = str.end();
	return wstring(a, b);
}
int TextBox::edit_begin() const { return min(edit_a, edit_b); }
int TextBox::edit_end() const { return max(edit_a, edit_b); }
bool TextBox::selected() const { return edit_a != edit_b; }
// 下面是对的，不需要加一减一什么的。
int TextBox::max_x_cs_rel() const { return w - w_edit_mg; }
int TextBox::x_cs_rel(App& app) const {
	wstring tmp(str.begin(), str.begin() + edit_b);
	return x_str_rel + str_wh(tmp, app.ft, 0).x;
}

void TextBox::select_a(App& app) {
	selecting = true;
	int x = app.input.msp.x - (tl_box().x + x_str_rel);
	edit_a = edit_b = str_index_at_pos(str, app.ft, { x, 0 }, 0);
}
void TextBox::select_b(App& app) {
	int x = app.input.msp.x - (tl_box().x + x_str_rel);
	edit_b = str_index_at_pos(str, app.ft, { x, 0 }, 0);
}

void TextBox::add_chs(App& app) {
	wstring sifted = FilterCharacters(app.input.chs, true);
	if (!sifted.empty()) {
		selecting = false;
		str = left() + sifted + right();
		edit_a = edit_b = edit_begin() + sifted.size();
	}
}
void TextBox::hdl_left(App& app) {
	selecting = false; 
	cursor_change = true;

	if (selected()) { edit_a = edit_b = edit_begin(); }
	else {
		if (--edit_a < 0) { edit_a = 0; }
		edit_b = edit_a;
	}
}
void TextBox::hdl_right(App& app) {
	selecting = false; 
	cursor_change = true;

	if (selected()) { edit_a = edit_b = edit_end(); }
	else {
		// 下面不要减一。
		if (++edit_a > str.size()) { edit_a = str.size(); }
		edit_b = edit_a;
	}
}
void TextBox::hdl_back(App& app) {
	selecting = false; 
	cursor_change = true;

	if (selected()) {
		str = left() + right();
		edit_a = edit_b = edit_begin();
	} else if (edit_begin() > 0) {
		x_str_rel += app.ft[str[edit_a]].width;
		x_str_rel = min<double>(x_str_rel, w_edit_mg);
		auto a = str.begin();
		auto b = str.begin() + edit_a - 1;
		str = wstring(a, b) + right();
		edit_b = --edit_a;
	}
}
void TextBox::hdl_delete(App& app) {
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
}
void TextBox::hdl_clip(App& app) {
	selecting = false;
	SetClipboard(mid());
	str = left() + right();
	edit_a = edit_b = edit_begin();
}
void TextBox::hdl_copy(App& app) { SetClipboard(mid()); }
void TextBox::hdl_paste(App& app) {
	selecting = false;
	wstring sifted = FilterCharacters(GetClipboard(), true);
	str = left() + sifted + right();
	edit_a = edit_b = edit_begin() + sifted.size();
}
void TextBox::hdl_all(App& app) {
	selecting = false;
	edit_a = 0; edit_b = str.size();
}
void TextBox::hdl_key(App& app) {
	cursor_change = false;

	if (app.input.kbmsg[VK_LEFT]) { hdl_left(app); }
	else if (app.input.kbmsg[VK_RIGHT]) { hdl_right(app); }
	else if (app.input.kbmsg[VK_DELETE]) { hdl_delete(app); }
	else if (app.input.kbmsg[VK_BACK]) { hdl_back(app); }

	if (app.input.kbd[VK_CONTROL]) {
		if (app.input.isKeyPressedDown('C')) { hdl_copy(app); }
		else if (app.input.isKeyPressedDown('X')) { hdl_clip(app); }
		else if (app.input.isKeyPressedDown('V')) { hdl_paste(app); }
		else if (app.input.isKeyPressedDown('A')) { hdl_all(app); }
	}
}

void TextBox::clamp_cs_selecting(App& app) {
	double val = (app.input.msp.x - tl_box().x) - max_x_cs_rel();
	if (val > 0) { x_str_rel -= app.fps.dt() * slide_mtp * val; }
	val = w_edit_mg - (app.input.msp.x - tl_box().x);
	if (val > 0) { x_str_rel += app.fps.dt() * slide_mtp * val; }
}
void TextBox::clamp_cs_not_selecting(App& app) {
	if (x_cs_rel(app) > max_x_cs_rel()) {
		x_str_rel -= x_cs_rel(app) - max_x_cs_rel();
	} else if (x_cs_rel(app) < w_edit_mg) {
		x_str_rel += w_edit_mg - x_cs_rel(app);
	}
}
void TextBox::clamp_cursor(App& app) {
	if (selecting) { clamp_cs_selecting(app); }
	else { clamp_cs_not_selecting(app); }
}
void TextBox::clamp_str(App& app) {
	int x_end_rel = x_str_rel + str_wh(str, app.ft, 0).x;
	if (x_end_rel < max_x_cs_rel()) {
		x_str_rel += max_x_cs_rel() - x_end_rel;
	}
	if (x_str_rel > w_edit_mg) { x_str_rel = w_edit_mg; }
}

void TextBox::render_main(App& app) {
	DVector2 tl_txt = tl + DVector2(0, (h - app.ft.h) / 2);
	DrawString(app.scr, app.dscr, depth, txt, c_txt, app.ft, tl_txt, 0, vp);

	Color const& c =
		!enabled ? c_invalid :
		edit ? c_edit :
		(hovered && !app.input.msd[0] && !app.input.msd[1] && !app.input.msd[2]) ? hoveredColor : c_normal;
	drawRectangleWithBorderRaw(app.scr, tl_box(), w, h, vp, c, c_frame);

	int x_cur = 0;
	DVector2 tl_str = tl_box() + DVector2(x_str_rel, (h - app.ft.h) / 2);
	DrawString(app.scr, app.dscr, depth, left(), c_str, app.ft, tl_str, x_cur, 0, vp_show_txt());
	DrawString(app.scr, app.dscr, depth, mid(), selectedColor, app.ft, tl_str, x_cur, 0, vp_show_txt());
	DrawString(app.scr, app.dscr, depth, right(), c_str, app.ft, tl_str, x_cur, 0, vp_show_txt());
}
void TextBox::render_cursor(App& app) {
	bool show_cursor = edit &&
		(sin(app.fps.dur() * twinkle_mtp) > 0 || cursor_change);
	if (show_cursor) {
		int x_cs = tl_box().x + x_cs_rel(app) - w_cursor / 2;
		int y_cs = tl_box().y + (h - h_cursor) / 2;
		drawRectangleRaw(app.scr, { x_cs, y_cs }, w_cursor, h_cursor, vp_show_txt(), c_cursor);
	}
}
void TextBox::render(App& app) {
	render_main(app); render_cursor(app);
}

void TextBox::Update(App& app) {
	hovered = (app.own.hovered == this);
	w_txt = str_wh(txt, app.ft, 0).x;
	if (edit) {
		if (selecting) { select_b(app); }
		if (enabled && hovered && app.input.isMouseClickedDown(0)) { select_a(app); }
		if (!app.input.msd[0]) { selecting = false; }
		hdl_key(app); add_chs(app); clamp_cursor(app); Upload(app);

		if (enabled && !hovered && app.input.isMouseClickedDown(0)) { OnDone(app); }
		if (!enabled || !hovered && app.input.isMouseClickedDown(0)) {
			// 请注意 kb_owner 的登记在 PreUpdate 中进行。
			app.own.free_kb(this); edit = selecting = false;
			edit_a = edit_b = 0;
		}
	} else {
		Sync(app);
		if (enabled && hovered && app.input.isMouseClickedDown(0)) {
			if (clr_upon_edit) { str.clear(); }
			select_a(app); edit = true;
		}
	} clamp_str(app); render(app);
}
void TextBox::Discard(App& app) {
	app.own.remove(this); app.own.free_kb(this);
	w_txt = 0; edit = hovered = false;
	selecting = cursor_change = false;
	edit_a = edit_b = 0;
}
void TextBox::PreUpdate(App& app) {
	if (edit) { app.own.keyboardOwner = this; }

	bool ok = app.own.hoveredDepth <= depth &&
		isInside(app.input.msp, { tl_box(), w, h }) && isInside(app.input.msp, vp);
	if (ok) { app.own.hoveredDepth = depth; app.own.hovered = this; }
}

LocalizedTextBox::LocalizedTextBox(wstring const& id, int w) : TextBox(w), id_txt(id) {}
void LocalizedTextBox::Update(App& app) { txt = app.getLocalizedString(id_txt); TextBox::Update(app); }



