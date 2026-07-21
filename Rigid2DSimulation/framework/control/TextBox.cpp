#include "TextBox.h"
#include "App.h"
#include "DrawGeometry.h"
#include "DrawString.h"
#include "DrawBorderedRectangle.h"
#include "RichTextBox.h"


TextBox::TextBox(int w) : w(w) {
	gap = { 10, 10 }; 
	depth = 1000; h = 30;

	labelTextColor = { 255, 255, 255 };
	valueColor = { 255, 255, 255 };
	editColor = { 150, 60, 60 };
	frameColor = { 150, 150, 150 };
	normalColor = { 40, 40, 40 };
	cursorColor = { 255, 255, 255 };
	hoveredColor = { 60, 60, 150 };
	invalidColor = { 0, 0, 0 };
	selectedColor = { 150, 150, 255 };

	editMarginWidth = 5;
	textViewportMarginWidth = 2;
	slideMultiplier = 8;
	blinkMultiplier = 8;
	cursorWidth = 2; cursorHeight = 20;
}
int TextBox::GetW() const { return w + gap.x + labelTextWidth; }
int TextBox::GetH() const { return h + gap.y; }

DVector2 TextBox::GetBoxTopLeft() const { return tl + DVector2(labelTextWidth, 0); }
DRect TextBox::GetTextViewport() const {
	DVector2 textViewportTopLeft = GetBoxTopLeft() + DVector2(textViewportMarginWidth, 0);
	int textViewportWidth = w - 2 * textViewportMarginWidth;
	return getOverlapRect(vp, { textViewportTopLeft, textViewportWidth, h });
}
wstring TextBox::GetLeftText() const {
	auto a = value.begin();
	auto b = value.begin() + GetEditBegin();
	return wstring(a, b);
}
wstring TextBox::GetSelectedText() const {
	auto a = value.begin() + GetEditBegin();
	auto b = value.begin() + GetEditEnd();
	return wstring(a, b);
}
wstring TextBox::GetRightText() const {
	auto a = value.begin() + GetEditEnd();
	auto b = value.end();
	return wstring(a, b);
}
int TextBox::GetEditBegin() const { return min(selectionStart, selectionEnd); }
int TextBox::GetEditEnd() const { return max(selectionStart, selectionEnd); }
bool TextBox::HasSelection() const { return selectionStart != selectionEnd; }
// 下面是对的，不需要加一减一什么的。
int TextBox::GetMaxCursorX() const { return w - editMarginWidth; }
int TextBox::GetCursorX(App& app) const {
	wstring tmp(value.begin(), value.begin() + selectionEnd);
	return valueOffsetX + GetStringSize(tmp, app.ft, 0).x;
}

void TextBox::SelectStart(App& app) {
	selecting = true;
	int x = app.input.mousePosition.x - (GetBoxTopLeft().x + valueOffsetX);
	selectionStart = selectionEnd = GetStringIndexAtPosition(value, app.ft, { x, 0 }, 0);
}
void TextBox::SelectEnd(App& app) {
	int x = app.input.mousePosition.x - (GetBoxTopLeft().x + valueOffsetX);
	selectionEnd = GetStringIndexAtPosition(value, app.ft, { x, 0 }, 0);
}

void TextBox::AddCharacters(App& app) {
	wstring sifted = FilterCharacters(app.input.typedCharacters, true);
	if (!sifted.empty()) {
		selecting = false;
		value = GetLeftText() + sifted + GetRightText();
		selectionStart = selectionEnd = GetEditBegin() + sifted.size();
	}
}
void TextBox::HandleLeft(App& app) {
	selecting = false; 
	cursor_change = true;

	if (HasSelection()) { selectionStart = selectionEnd = GetEditBegin(); }
	else {
		if (--selectionStart < 0) { selectionStart = 0; }
		selectionEnd = selectionStart;
	}
}
void TextBox::HandleRight(App& app) {
	selecting = false; 
	cursor_change = true;

	if (HasSelection()) { selectionStart = selectionEnd = GetEditEnd(); }
	else {
		// 下面不要减一。
		if (++selectionStart > value.size()) { selectionStart = value.size(); }
		selectionEnd = selectionStart;
	}
}
void TextBox::HandleBackspace(App& app) {
	selecting = false; 
	cursor_change = true;

	if (HasSelection()) {
		value = GetLeftText() + GetRightText();
		selectionStart = selectionEnd = GetEditBegin();
	} else if (GetEditBegin() > 0) {
		valueOffsetX += app.ft[value[selectionStart]].width;
		valueOffsetX = min<double>(valueOffsetX, editMarginWidth);
		auto a = value.begin();
		auto b = value.begin() + selectionStart - 1;
		value = wstring(a, b) + GetRightText();
		selectionEnd = --selectionStart;
	}
}
void TextBox::HandleDelete(App& app) {
	selecting = false; 
	cursor_change = true;

	if (HasSelection()) {
		value = GetLeftText() + GetRightText();
		selectionStart = selectionEnd = GetEditBegin();
	} else if (GetEditEnd() < value.size()) {
		auto a = value.begin() + selectionStart + 1;
		auto b = value.end();
		value = GetLeftText() + wstring(a, b);
	}
}
void TextBox::HandleCut(App& app) {
	selecting = false;
	SetClipboard(GetSelectedText());
	value = GetLeftText() + GetRightText();
	selectionStart = selectionEnd = GetEditBegin();
}
void TextBox::HandleCopy(App& app) { SetClipboard(GetSelectedText()); }
void TextBox::HandlePaste(App& app) {
	selecting = false;
	wstring sifted = FilterCharacters(GetClipboard(), true);
	value = GetLeftText() + sifted + GetRightText();
	selectionStart = selectionEnd = GetEditBegin() + sifted.size();
}
void TextBox::HandleSelectAll(App& app) {
	selecting = false;
	selectionStart = 0; selectionEnd = value.size();
}
void TextBox::HandleKey(App& app) {
	cursor_change = false;

	if (app.input.keyMessageCounts[VK_LEFT]) { HandleLeft(app); }
	else if (app.input.keyMessageCounts[VK_RIGHT]) { HandleRight(app); }
	else if (app.input.keyMessageCounts[VK_DELETE]) { HandleDelete(app); }
	else if (app.input.keyMessageCounts[VK_BACK]) { HandleBackspace(app); }

	if (app.input.keyDown[VK_CONTROL]) {
		if (app.input.IsKeyPressedDown('C')) { HandleCopy(app); }
		else if (app.input.IsKeyPressedDown('X')) { HandleCut(app); }
		else if (app.input.IsKeyPressedDown('V')) { HandlePaste(app); }
		else if (app.input.IsKeyPressedDown('A')) { HandleSelectAll(app); }
	}
}

void TextBox::ClampSelectingCursor(App& app) {
	double val = (app.input.mousePosition.x - GetBoxTopLeft().x) - GetMaxCursorX();
	if (val > 0) { valueOffsetX -= app.fps.GetDeltaTime() * slideMultiplier * val; }
	val = editMarginWidth - (app.input.mousePosition.x - GetBoxTopLeft().x);
	if (val > 0) { valueOffsetX += app.fps.GetDeltaTime() * slideMultiplier * val; }
}
void TextBox::ClampCursor(App& app) {
	if (GetCursorX(app) > GetMaxCursorX()) {
		valueOffsetX -= GetCursorX(app) - GetMaxCursorX();
	} else if (GetCursorX(app) < editMarginWidth) {
		valueOffsetX += editMarginWidth - GetCursorX(app);
	}
}
void TextBox::ClampCursorPosition(App& app) {
	if (selecting) { ClampSelectingCursor(app); }
	else { ClampCursor(app); }
}
void TextBox::ClampTextPosition(App& app) {
	int valueEndX = valueOffsetX + GetStringSize(value, app.ft, 0).x;
	if (valueEndX < GetMaxCursorX()) {
		valueOffsetX += GetMaxCursorX() - valueEndX;
	}
	if (valueOffsetX > editMarginWidth) { valueOffsetX = editMarginWidth; }
}

void TextBox::RenderMain(App& app) {
	DVector2 labelTextTopLeft = tl + DVector2(0, (h - app.ft.h) / 2);
	DrawString(app.scr, app.dscr, depth, labelText, labelTextColor, app.ft, labelTextTopLeft, 0, vp);

	Color const& color =
		!enabled ? invalidColor :
		edit ? editColor :
		(hovered && !app.input.mouseButtonDown[0] && !app.input.mouseButtonDown[1] && !app.input.mouseButtonDown[2]) ? hoveredColor : normalColor;
	drawRectangleWithBorderRaw(app.scr, GetBoxTopLeft(), w, h, vp, color, frameColor);

	int cursorX = 0;
	DVector2 valueTopLeft = GetBoxTopLeft() + DVector2(valueOffsetX, (h - app.ft.h) / 2);
	DrawString(app.scr, app.dscr, depth, GetLeftText(), valueColor, app.ft, valueTopLeft, cursorX, 0, GetTextViewport());
	DrawString(app.scr, app.dscr, depth, GetSelectedText(), selectedColor, app.ft, valueTopLeft, cursorX, 0, GetTextViewport());
	DrawString(app.scr, app.dscr, depth, GetRightText(), valueColor, app.ft, valueTopLeft, cursorX, 0, GetTextViewport());
}
void TextBox::RenderCursor(App& app) {
	bool show_cursor = edit &&
		(sin(app.fps.GetElapsedTime() * blinkMultiplier) > 0 || cursor_change);
	if (show_cursor) {
		int cursorScreenX = GetBoxTopLeft().x + GetCursorX(app) - cursorWidth / 2;
		int cursorScreenY = GetBoxTopLeft().y + (h - cursorHeight) / 2;
		drawRectangleRaw(app.scr, { cursorScreenX, cursorScreenY }, cursorWidth, cursorHeight, GetTextViewport(), cursorColor);
	}
}
void TextBox::Render(App& app) {
	RenderMain(app); RenderCursor(app);
}

void TextBox::Update(App& app) {
	hovered = (app.own.hovered == this);
	labelTextWidth = GetStringSize(labelText, app.ft, 0).x;
	if (edit) {
		if (selecting) { SelectEnd(app); }
		if (enabled && hovered && app.input.IsMouseClickedDown(0)) { SelectStart(app); }
		if (!app.input.mouseButtonDown[0]) { selecting = false; }
		HandleKey(app); AddCharacters(app); ClampCursorPosition(app); Upload(app);

		if (enabled && !hovered && app.input.IsMouseClickedDown(0)) { OnDone(app); }
		if (!enabled || !hovered && app.input.IsMouseClickedDown(0)) {
			// 请注意 kb_owner 的登记在 PreUpdate 中进行。
			app.own.ReleaseKeyboard(this); edit = selecting = false;
			selectionStart = selectionEnd = 0;
		}
	} else {
		Sync(app);
		if (enabled && hovered && app.input.IsMouseClickedDown(0)) {
			if (clearUponEdit) { value.clear(); }
			SelectStart(app); edit = true;
		}
	} ClampTextPosition(app); Render(app);
}
void TextBox::Discard(App& app) {
	app.own.Remove(this); app.own.ReleaseKeyboard(this);
	labelTextWidth = 0; edit = hovered = false;
	selecting = cursor_change = false;
	selectionStart = selectionEnd = 0;
}
void TextBox::PreUpdate(App& app) {
	if (edit) { app.own.keyboardOwner = this; }

	bool ok = app.own.hoveredDepth <= depth &&
		isInside(app.input.mousePosition, { GetBoxTopLeft(), w, h }) && isInside(app.input.mousePosition, vp);
	if (ok) { app.own.hoveredDepth = depth; app.own.hovered = this; }
}

LocalizedTextBox::LocalizedTextBox(wstring const& id, int w) : TextBox(w), labelTextId(id) {}
void LocalizedTextBox::Update(App& app) { labelText = app.GetLocalizedString(labelTextId); TextBox::Update(app); }



