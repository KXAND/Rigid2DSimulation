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

DVector2 TextBox::GetBoxTopLeft() const { return tl + DVector2(w_txt, 0); }
DRect TextBox::GetTextViewport() const {
	DVector2 tl_show_txt = GetBoxTopLeft() + DVector2(w_show_txt_mg, 0);
	int w_show_txt = w - 2 * w_show_txt_mg;
	return getOverlapRect(vp, { tl_show_txt, w_show_txt, h });
}
wstring TextBox::GetLeftText() const {
	auto a = str.begin();
	auto b = str.begin() + GetEditBegin();
	return wstring(a, b);
}
wstring TextBox::GetSelectedText() const {
	auto a = str.begin() + GetEditBegin();
	auto b = str.begin() + GetEditEnd();
	return wstring(a, b);
}
wstring TextBox::GetRightText() const {
	auto a = str.begin() + GetEditEnd();
	auto b = str.end();
	return wstring(a, b);
}
int TextBox::GetEditBegin() const { return min(edit_a, edit_b); }
int TextBox::GetEditEnd() const { return max(edit_a, edit_b); }
bool TextBox::HasSelection() const { return edit_a != edit_b; }
// 下面是对的，不需要加一减一什么的。
int TextBox::GetMaxCursorX() const { return w - w_edit_mg; }
int TextBox::GetCursorX(App& app) const {
	wstring tmp(str.begin(), str.begin() + edit_b);
	return x_str_rel + GetStringSize(tmp, app.ft, 0).x;
}

void TextBox::SelectStart(App& app) {
	selecting = true;
	int x = app.input.msp.x - (GetBoxTopLeft().x + x_str_rel);
	edit_a = edit_b = GetStringIndexAtPosition(str, app.ft, { x, 0 }, 0);
}
void TextBox::SelectEnd(App& app) {
	int x = app.input.msp.x - (GetBoxTopLeft().x + x_str_rel);
	edit_b = GetStringIndexAtPosition(str, app.ft, { x, 0 }, 0);
}

void TextBox::AddCharacters(App& app) {
	wstring sifted = FilterCharacters(app.input.chs, true);
	if (!sifted.empty()) {
		selecting = false;
		str = GetLeftText() + sifted + GetRightText();
		edit_a = edit_b = GetEditBegin() + sifted.size();
	}
}
void TextBox::HandleLeft(App& app) {
	selecting = false; 
	cursor_change = true;

	if (HasSelection()) { edit_a = edit_b = GetEditBegin(); }
	else {
		if (--edit_a < 0) { edit_a = 0; }
		edit_b = edit_a;
	}
}
void TextBox::HandleRight(App& app) {
	selecting = false; 
	cursor_change = true;

	if (HasSelection()) { edit_a = edit_b = GetEditEnd(); }
	else {
		// 下面不要减一。
		if (++edit_a > str.size()) { edit_a = str.size(); }
		edit_b = edit_a;
	}
}
void TextBox::HandleBackspace(App& app) {
	selecting = false; 
	cursor_change = true;

	if (HasSelection()) {
		str = GetLeftText() + GetRightText();
		edit_a = edit_b = GetEditBegin();
	} else if (GetEditBegin() > 0) {
		x_str_rel += app.ft[str[edit_a]].width;
		x_str_rel = min<double>(x_str_rel, w_edit_mg);
		auto a = str.begin();
		auto b = str.begin() + edit_a - 1;
		str = wstring(a, b) + GetRightText();
		edit_b = --edit_a;
	}
}
void TextBox::HandleDelete(App& app) {
	selecting = false; 
	cursor_change = true;

	if (HasSelection()) {
		str = GetLeftText() + GetRightText();
		edit_a = edit_b = GetEditBegin();
	} else if (GetEditEnd() < str.size()) {
		auto a = str.begin() + edit_a + 1;
		auto b = str.end();
		str = GetLeftText() + wstring(a, b);
	}
}
void TextBox::HandleCut(App& app) {
	selecting = false;
	SetClipboard(GetSelectedText());
	str = GetLeftText() + GetRightText();
	edit_a = edit_b = GetEditBegin();
}
void TextBox::HandleCopy(App& app) { SetClipboard(GetSelectedText()); }
void TextBox::HandlePaste(App& app) {
	selecting = false;
	wstring sifted = FilterCharacters(GetClipboard(), true);
	str = GetLeftText() + sifted + GetRightText();
	edit_a = edit_b = GetEditBegin() + sifted.size();
}
void TextBox::HandleSelectAll(App& app) {
	selecting = false;
	edit_a = 0; edit_b = str.size();
}
void TextBox::HandleKey(App& app) {
	cursor_change = false;

	if (app.input.kbmsg[VK_LEFT]) { HandleLeft(app); }
	else if (app.input.kbmsg[VK_RIGHT]) { HandleRight(app); }
	else if (app.input.kbmsg[VK_DELETE]) { HandleDelete(app); }
	else if (app.input.kbmsg[VK_BACK]) { HandleBackspace(app); }

	if (app.input.kbd[VK_CONTROL]) {
		if (app.input.isKeyPressedDown('C')) { HandleCopy(app); }
		else if (app.input.isKeyPressedDown('X')) { HandleCut(app); }
		else if (app.input.isKeyPressedDown('V')) { HandlePaste(app); }
		else if (app.input.isKeyPressedDown('A')) { HandleSelectAll(app); }
	}
}

void TextBox::ClampSelectingCursor(App& app) {
	double val = (app.input.msp.x - GetBoxTopLeft().x) - GetMaxCursorX();
	if (val > 0) { x_str_rel -= app.fps.dt() * slide_mtp * val; }
	val = w_edit_mg - (app.input.msp.x - GetBoxTopLeft().x);
	if (val > 0) { x_str_rel += app.fps.dt() * slide_mtp * val; }
}
void TextBox::ClampCursor(App& app) {
	if (GetCursorX(app) > GetMaxCursorX()) {
		x_str_rel -= GetCursorX(app) - GetMaxCursorX();
	} else if (GetCursorX(app) < w_edit_mg) {
		x_str_rel += w_edit_mg - GetCursorX(app);
	}
}
void TextBox::ClampCursorPosition(App& app) {
	if (selecting) { ClampSelectingCursor(app); }
	else { ClampCursor(app); }
}
void TextBox::ClampTextPosition(App& app) {
	int x_end_rel = x_str_rel + GetStringSize(str, app.ft, 0).x;
	if (x_end_rel < GetMaxCursorX()) {
		x_str_rel += GetMaxCursorX() - x_end_rel;
	}
	if (x_str_rel > w_edit_mg) { x_str_rel = w_edit_mg; }
}

void TextBox::RenderMain(App& app) {
	DVector2 tl_txt = tl + DVector2(0, (h - app.ft.h) / 2);
	DrawString(app.scr, app.dscr, depth, txt, c_txt, app.ft, tl_txt, 0, vp);

	Color const& c =
		!enabled ? c_invalid :
		edit ? c_edit :
		(hovered && !app.input.msd[0] && !app.input.msd[1] && !app.input.msd[2]) ? hoveredColor : c_normal;
	drawRectangleWithBorderRaw(app.scr, GetBoxTopLeft(), w, h, vp, c, c_frame);

	int x_cur = 0;
	DVector2 tl_str = GetBoxTopLeft() + DVector2(x_str_rel, (h - app.ft.h) / 2);
	DrawString(app.scr, app.dscr, depth, GetLeftText(), c_str, app.ft, tl_str, x_cur, 0, GetTextViewport());
	DrawString(app.scr, app.dscr, depth, GetSelectedText(), selectedColor, app.ft, tl_str, x_cur, 0, GetTextViewport());
	DrawString(app.scr, app.dscr, depth, GetRightText(), c_str, app.ft, tl_str, x_cur, 0, GetTextViewport());
}
void TextBox::RenderCursor(App& app) {
	bool show_cursor = edit &&
		(sin(app.fps.dur() * twinkle_mtp) > 0 || cursor_change);
	if (show_cursor) {
		int x_cs = GetBoxTopLeft().x + GetCursorX(app) - w_cursor / 2;
		int y_cs = GetBoxTopLeft().y + (h - h_cursor) / 2;
		drawRectangleRaw(app.scr, { x_cs, y_cs }, w_cursor, h_cursor, GetTextViewport(), c_cursor);
	}
}
void TextBox::Render(App& app) {
	RenderMain(app); RenderCursor(app);
}

void TextBox::Update(App& app) {
	hovered = (app.own.hovered == this);
	w_txt = GetStringSize(txt, app.ft, 0).x;
	if (edit) {
		if (selecting) { SelectEnd(app); }
		if (enabled && hovered && app.input.isMouseClickedDown(0)) { SelectStart(app); }
		if (!app.input.msd[0]) { selecting = false; }
		HandleKey(app); AddCharacters(app); ClampCursorPosition(app); Upload(app);

		if (enabled && !hovered && app.input.isMouseClickedDown(0)) { OnDone(app); }
		if (!enabled || !hovered && app.input.isMouseClickedDown(0)) {
			// 请注意 kb_owner 的登记在 PreUpdate 中进行。
			app.own.ReleaseKeyboard(this); edit = selecting = false;
			edit_a = edit_b = 0;
		}
	} else {
		Sync(app);
		if (enabled && hovered && app.input.isMouseClickedDown(0)) {
			if (clr_upon_edit) { str.clear(); }
			SelectStart(app); edit = true;
		}
	} ClampTextPosition(app); Render(app);
}
void TextBox::Discard(App& app) {
	app.own.remove(this); app.own.ReleaseKeyboard(this);
	w_txt = 0; edit = hovered = false;
	selecting = cursor_change = false;
	edit_a = edit_b = 0;
}
void TextBox::PreUpdate(App& app) {
	if (edit) { app.own.keyboardOwner = this; }

	bool ok = app.own.hoveredDepth <= depth &&
		isInside(app.input.msp, { GetBoxTopLeft(), w, h }) && isInside(app.input.msp, vp);
	if (ok) { app.own.hoveredDepth = depth; app.own.hovered = this; }
}

LocalizedTextBox::LocalizedTextBox(wstring const& id, int w) : TextBox(w), id_txt(id) {}
void LocalizedTextBox::Update(App& app) { txt = app.getLocalizedString(id_txt); TextBox::Update(app); }



