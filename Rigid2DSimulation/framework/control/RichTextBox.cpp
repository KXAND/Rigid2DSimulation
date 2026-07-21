#include "RichTextBox.h"
#include "App.h"
#include "DrawGeometry.h"
#include "DrawString.h"
#include "DrawBorderedRectangle.h"


RichTextBox::RichTextBox() : RichTextBox(0, 0) {}
RichTextBox::RichTextBox(int w, int h, bool newline) : w(w), h(h) {
	gap = { 10, 10 }; depth = 1000;

	valueColor = { 255, 255, 255 };
	editColor = { 80, 40, 40 };
	frameColor = { 150, 150, 150 };
	normalColor = { 40, 40, 40 };
	cursorColor = { 255, 255, 255 };
	hoveredColor = { 40, 40, 80 };
	invalidColor = { 0, 0, 0 };
	selectedColor = { 150, 150, 255 };

	editMargin = { 5, 5 };
	textViewportMargin = { 2, 2 };
	slideMultiplier = 8;
	wheelMultiplier = 40;
	blinkMultiplier = 8;
	cursorWidth = 2; cursorHeight = 20;

	if (newline) { lineWidth = w - 2 * editMargin.x; }
}
int RichTextBox::GetW() const { return w + gap.x; }
int RichTextBox::GetH() const { return h + gap.y; }

DRect RichTextBox::GetTextViewport() const {
	DVector2 textViewportTopLeft = tl + textViewportMargin;
	int textViewportWidth = w - 2 * textViewportMargin.x;
	int textViewportHeight = h - 2 * textViewportMargin.y;
	return getOverlapRect(vp, { textViewportTopLeft, textViewportWidth, textViewportHeight });
}
wstring RichTextBox::GetLeftText() const {
	auto valueBegin = value.begin();
	auto selectionBegin = value.begin() + GetEditBegin();
	return wstring(valueBegin, selectionBegin);
}
wstring RichTextBox::GetSelectedText() const {
	auto selectionBegin = value.begin() + GetEditBegin();
	auto selectionEnd = value.begin() + GetEditEnd();
	return wstring(selectionBegin, selectionEnd);
}
wstring RichTextBox::GetRightText() const {
	auto selectionEnd = value.begin() + GetEditEnd();
	auto valueEnd = value.end();
	return wstring(selectionEnd, valueEnd);
}
int RichTextBox::GetEditBegin() const { return min(selectionStart, selectionEnd); }
int RichTextBox::GetEditEnd() const { return max(selectionStart, selectionEnd); }
bool RichTextBox::HasSelection() const { return selectionStart != selectionEnd; }
int RichTextBox::GetMaxCursorX() const { return w - editMargin.x; }
int RichTextBox::GetMaxCursorY() const { return h - editMargin.y; }
DVector2 RichTextBox::GetCursorPosition(App& app) const {
	wstring selectedText(value.begin(), value.begin() + selectionEnd);
	int cursorX = 0; DVector2 cursorPosition = GetStringSize(selectedText, app.ft, lineWidth, &cursorX);
	cursorPosition.x = cursorX; cursorPosition.y -= app.ft.h;
	return (DVector2)valueOffset + cursorPosition;
}

void RichTextBox::SelectStart(App& app) {
	selecting = true;
	DVector2 mousePosition = app.input.msp - (tl + (DVector2)valueOffset);
	selectionStart = selectionEnd = GetStringIndexAtPosition(value, app.ft, mousePosition, lineWidth);
	UpdateCursorXHistory(app);
}
void RichTextBox::SelectEnd(App& app) {
	DVector2 mousePosition = app.input.msp - (tl + (DVector2)valueOffset);
	selectionEnd = GetStringIndexAtPosition(value, app.ft, mousePosition, lineWidth);
	UpdateCursorXHistory(app);
}
void RichTextBox::UpdateCursorXHistory(App& app) {
	auto textBeforeCursor = wstring(value.begin(), value.begin() + selectionEnd);
	int cursorX = 0; GetStringSize(textBeforeCursor, app.ft, lineWidth, &cursorX);
	cursorXHistory = cursorX;
}

void RichTextBox::AddCharacters(App& app) {
	wstring sifted = FilterCharacters(app.input.chs, false);
	if (!sifted.empty()) {
		selecting = false;
		value = GetLeftText() + sifted + GetRightText();
		selectionStart = selectionEnd = GetEditBegin() + sifted.size();
		UpdateCursorXHistory(app);
	}
}
void RichTextBox::HandleLeft(App& app) {
	selecting = false;
	cursorChanged = true;

	if (HasSelection()) { selectionStart = selectionEnd = GetEditBegin(); }
	else {
		if (--selectionStart < 0) { selectionStart = 0; }
		selectionEnd = selectionStart;
	} UpdateCursorXHistory(app);
}
void RichTextBox::HandleRight(App& app) {
	selecting = false;
	cursorChanged = true;

	if (HasSelection()) { selectionStart = selectionEnd = GetEditEnd(); }
	else {
		// 下面不要减一。
		if (++selectionStart > value.size()) { selectionStart = value.size(); }
		selectionEnd = selectionStart;
	} UpdateCursorXHistory(app);
}
void RichTextBox::HandleUp(App& app) {
	selecting = false;
	cursorChanged = true;

	if (HasSelection()) { selectionStart = selectionEnd = GetEditBegin(); }
	else {
		wstring selectedText(value.begin(), value.begin() + selectionEnd);
		int cursorY = GetStringSize(selectedText, app.ft, lineWidth).y - 2 * app.ft.h;
		DVector2 cursorPosition(cursorXHistory, cursorY);
		selectionStart = selectionEnd = GetStringIndexAtPosition(value, app.ft, cursorPosition, lineWidth);
	}
}
void RichTextBox::HandleDown(App& app) {
	selecting = false;
	cursorChanged = true;

	if (HasSelection()) { selectionStart = selectionEnd = GetEditEnd(); }
	else {
		wstring selectedText(value.begin(), value.begin() + selectionEnd);
		int cursorY = GetStringSize(selectedText, app.ft, lineWidth).y;
		DVector2 cursorPosition(cursorXHistory, cursorY);
		selectionStart = selectionEnd = GetStringIndexAtPosition(value, app.ft, cursorPosition, lineWidth);
	}
}
void RichTextBox::HandleBackspace(App& app) {
	selecting = false;
	cursorChanged = true;

	if (HasSelection()) {
		value = GetLeftText() + GetRightText();
		selectionStart = selectionEnd = GetEditBegin();
	} else if (GetEditBegin() > 0) {
		auto valueBegin = value.begin();
		auto leftTextEnd = value.begin() + selectionStart - 1;
		value = wstring(valueBegin, leftTextEnd) + GetRightText();
		selectionEnd = --selectionStart;
	} UpdateCursorXHistory(app);
}
void RichTextBox::HandleDelete(App& app) {
	selecting = false;
	cursorChanged = true;

	if (HasSelection()) {
		value = GetLeftText() + GetRightText();
		selectionStart = selectionEnd = GetEditBegin();
	} else if (GetEditEnd() < value.size()) {
		auto rightTextBegin = value.begin() + selectionStart + 1;
		auto valueEnd = value.end();
		value = GetLeftText() + wstring(rightTextBegin, valueEnd);
	}
	// 按道理 delete 不用更新的。
	// UpdateCursorXHistory(app);
}
void RichTextBox::HandleCut(App& app) {
	selecting = false;
	SetClipboard(GetSelectedText());
	value = GetLeftText() + GetRightText();
	selectionStart = selectionEnd = GetEditBegin();
	UpdateCursorXHistory(app);
}
void RichTextBox::HandleCopy(App& app) { SetClipboard(GetSelectedText()); }
void RichTextBox::HandlePaste(App& app) {
	selecting = false;
	wstring sifted = FilterCharacters(GetClipboard(), false);
	value = GetLeftText() + sifted + GetRightText();
	selectionStart = selectionEnd = GetEditBegin() + sifted.size();
	UpdateCursorXHistory(app);
}
void RichTextBox::HandleSelectAll(App& app) {
	selecting = false;
	selectionStart = 0; selectionEnd = value.size();
	UpdateCursorXHistory(app);
}
void RichTextBox::HandleKey(App& app) {
	cursorChanged = false;

	if (app.input.kbmsg[VK_LEFT]) { HandleLeft(app); }
	else if (app.input.kbmsg[VK_RIGHT]) { HandleRight(app); }
	else if (app.input.kbmsg[VK_UP]) { HandleUp(app); }
	else if (app.input.kbmsg[VK_DOWN]) { HandleDown(app); }
	else if (app.input.kbmsg[VK_DELETE]) { HandleDelete(app); }
	else if (app.input.kbmsg[VK_BACK]) { HandleBackspace(app); }

	if (app.input.kbd[VK_CONTROL]) {
		if (app.input.isKeyPressedDown('C')) { HandleCopy(app); }
		else if (app.input.isKeyPressedDown('X')) { HandleCut(app); }
		else if (app.input.isKeyPressedDown('V')) { HandlePaste(app); }
		else if (app.input.isKeyPressedDown('A')) { HandleSelectAll(app); }
	}
}

void RichTextBox::ClampCursorWhileSelecting(App& app) {
	double overflowDistance = (app.input.msp.x - tl.x) - GetMaxCursorX();
	if (overflowDistance > 0) { valueOffset.x -= app.fps.dt() * slideMultiplier * overflowDistance; }
	overflowDistance = editMargin.x - (app.input.msp.x - tl.x);
	if (overflowDistance > 0) { valueOffset.x += app.fps.dt() * slideMultiplier * overflowDistance; }
	overflowDistance = (app.input.msp.y - tl.y) - GetMaxCursorY();
	if (overflowDistance > 0) { valueOffset.y -= app.fps.dt() * slideMultiplier * overflowDistance; }
	overflowDistance = editMargin.y - (app.input.msp.y - tl.y);
	if (overflowDistance > 0) { valueOffset.y += app.fps.dt() * slideMultiplier * overflowDistance; }
}
void RichTextBox::ClampCursorWithoutSelection(App& app) {
	if (GetCursorPosition(app).x > GetMaxCursorX()) {
		valueOffset.x -= GetCursorPosition(app).x - GetMaxCursorX();
	} else if (GetCursorPosition(app).x < editMargin.x) {
		valueOffset.x += editMargin.x - GetCursorPosition(app).x;
	}
	// 我们不对 y 这么做，主要是为了让鼠标滚轮正常，其实细想挺奇怪的。
}
void RichTextBox::ClampCursor(App& app) {
	if (selecting) { ClampCursorWhileSelecting(app); }
	else { ClampCursorWithoutSelection(app); }
}
void RichTextBox::ClampTextPosition(App& app) {
	// 除了文字过少以外，clamp_selecting 和 wheel 后也需要这一步。
	DVector2 textEndPosition = (DVector2)valueOffset + GetStringSize(value, app.ft, lineWidth);
	if (textEndPosition.x < GetMaxCursorX()) {
		valueOffset.x += GetMaxCursorX() - textEndPosition.x;
	}
	if (textEndPosition.y < GetMaxCursorY()) {
		valueOffset.y += GetMaxCursorY() - textEndPosition.y;
	}
	// 下面两行是配合上面的。
	if (valueOffset.x > editMargin.x) { valueOffset.x = editMargin.x; }
	if (valueOffset.y > editMargin.y) { valueOffset.y = editMargin.y; }
}

void RichTextBox::RenderMain(App& app) {
	Color color =
		!enabled ? invalidColor :
		edit ? editColor :
		(hovered && !app.input.msd[0] && !app.input.msd[1] && !app.input.msd[2]) ? hoveredColor : normalColor;
	drawRectangleWithBorderRaw(app.scr, tl, w, h, vp, color, frameColor);

	int textCursorX = 0;
	DVector2 textTopLeft = tl + (DVector2)valueOffset;
	DrawString(app.scr, app.dscr, depth, GetLeftText(), valueColor, app.ft, textTopLeft, textCursorX, lineWidth, GetTextViewport());
	DrawString(app.scr, app.dscr, depth, GetSelectedText(), selectedColor, app.ft, textTopLeft, textCursorX, lineWidth, GetTextViewport());
	DrawString(app.scr, app.dscr, depth, GetRightText(), valueColor, app.ft, textTopLeft, textCursorX, lineWidth, GetTextViewport());
}
void RichTextBox::RenderCursor(App& app) {
	bool showCursor = edit &&
		(sin(app.fps.dur() * blinkMultiplier) > 0 || cursorChanged);
	if (showCursor) {
		DVector2 cursorTopLeft = tl + GetCursorPosition(app) + DVector2(-cursorWidth, app.ft.h - cursorHeight) / 2;
		drawRectangleRaw(app.scr, cursorTopLeft, cursorWidth, cursorHeight, GetTextViewport(), cursorColor);
	}
}
void RichTextBox::Render(App& app) {
	RenderMain(app); RenderCursor(app);
}

void RichTextBox::Update(App& app) {
	hovered = (app.own.hovered == this);
	if (edit) {
		// 我感觉 selecting 有关的逻辑也是有点乱的。
		// 我感觉 selecting 有关的逻辑也是有点乱的。
		// 怎么看待 UI 交互真的是个大问题。怎么清晰的表达需求。
		if (selecting) { SelectEnd(app); }
		if (enabled && hovered && app.input.isMouseClickedDown(0)) { SelectStart(app); }
		if (!app.input.msd[0]) { selecting = false; }
		HandleKey(app); AddCharacters(app); ClampCursor(app); Upload(app);

		if (enabled && !hovered && app.input.isMouseClickedDown(0)) { OnDone(app); }
		if (!enabled || !hovered && app.input.isMouseClickedDown(0)) {
			// 请注意 kb_owner 的登记在 PreUpdate 中进行。
			app.own.ReleaseKeyboard(this); edit = selecting = false;
			selectionStart = selectionEnd = 0; cursorXHistory = 0;
		}
	} else {
		Sync(app);
		if (enabled && hovered && app.input.isMouseClickedDown(0)) {
			if (clearUponEdit) { value.clear(); }
			SelectStart(app); edit = true;
		}
	} 

	if (app.own.wheeled == this) { valueOffset.y += app.input.msw * wheelMultiplier; }
	ClampTextPosition(app); Render(app);
}
void RichTextBox::Discard(App& app) {
	app.own.remove(this); app.own.ReleaseKeyboard(this);
	edit = hovered = false; selecting = false;
	selectionStart = selectionEnd = 0; cursorXHistory = 0;
}
void RichTextBox::PreUpdate(App& app) {
	if (edit) { app.own.keyboardOwner = this; }

	bool ok = app.own.hoveredDepth <= depth &&
		isInside(app.input.msp, { tl, w, h }) && isInside(app.input.msp, vp);
	if (ok) { app.own.hoveredDepth = depth; app.own.hovered = this; }

	ok = app.own.wheeledDepth <= depth &&
		isInside(app.input.msp, { tl, w, h }) && isInside(app.input.msp, vp);
	if (ok) { app.own.wheeledDepth = depth; app.own.wheeled = this; }
}



