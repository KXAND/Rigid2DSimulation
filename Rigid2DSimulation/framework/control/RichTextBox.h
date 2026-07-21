#pragma once
#include "Vector2.h"
#include "Font.h"
#include "Control.h"

struct RichTextBox : Control {
	DVector2 gap;
	int lineWidth = 0;
	double depth = 0;
	int w = 0, h = 0;
	bool clr_upon_edit = false;
	Color c_str, c_edit, c_frame, c_normal,
		c_cursor, hoveredColor, c_invalid, selectedColor;

	DVector2 edit_mg;
	DVector2 show_txt_mg;
	double slide_mtp = 0;
	double wheel_mtp = 0;
	double twinkle_mtp = 0;
	int w_cursor = 0, h_cursor = 0;

	wstring value;
	Vector2 tl_str_rel;
	int selectionStart = 0, selectionEnd = 0;
	// 下面的 rel 是相对于 tl_str_rel 的。
	int x_cs_rel_hist = 0;

	bool edit = false;
	bool enabled = true;
	bool hovered = false;
	bool selecting = false;
	bool cursor_change = false;

	RichTextBox();
	RichTextBox(int w, int h, bool newline = true);
	int GetW() const override;
	int GetH() const override;

	DRect GetTextViewport() const;
	wstring GetLeftText() const;
	wstring GetSelectedText() const;
	wstring GetRightText() const;
	int GetEditBegin() const;
	int GetEditEnd() const;
	bool HasSelection() const;
	int GetMaxCursorX() const;
	int GetMaxCursorY() const;
	DVector2 GetCursorPosition(App& app) const;

	void SelectStart(App& app);
	void SelectEnd(App& app);
	void UpdateCursorXHistory(App& app);

	void AddCharacters(App& app);
	void HandleLeft(App& app);
	void HandleRight(App& app);
	void HandleUp(App& app);
	void HandleDown(App& app);
	void HandleBackspace(App& app);
	void HandleDelete(App& app);
	void HandleCut(App& app);
	void HandleCopy(App& app);
	void HandlePaste(App& app);
	void HandleSelectAll(App& app);
	void HandleKey(App& app);

	void ClampCursorWhileSelecting(App& app);
	void ClampCursorWithoutSelection(App& app);
	void ClampCursor(App& app);
	void ClampTextPosition(App& app);

	void RenderMain(App& app);
	void RenderCursor(App& app);
	void Render(App& app);

	virtual void Sync(App& app) {}
	virtual void OnDone(App& app) const {}
	virtual void Upload(App& app) const {}

	void Update(App& app) override;
	void Discard(App& app) override;
	void PreUpdate(App& app) override;
};


