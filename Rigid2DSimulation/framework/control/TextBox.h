#pragma once
#include "Font.h"
#include "Control.h"

struct TextBox : Control {
	DVector2 gap;
	double depth = 0;
	wstring txt;
	int w = 0, h = 0;
	bool clr_upon_edit = false;
	Color c_txt, c_str, c_edit, c_frame, c_normal,
		c_cursor, hoveredColor, c_invalid, selectedColor;

	// cs = cursor，mg = margin。
	// 也是 x_str_rel 的默认值，否则一开始编辑文字的位置就会变化。
	int w_edit_mg = 0;
	int w_show_txt_mg = 0;
	double slide_mtp = 0;
	double twinkle_mtp = 0;
	int w_cursor = 0, h_cursor = 0;

	wstring str;
	double x_str_rel = 0;
	int edit_a = 0, edit_b = 0;

	int w_txt = 0;
	bool edit = false;
	bool enabled = true;
	bool hovered = false;
	bool selecting = false;
	bool cursor_change = false;

	TextBox(int w = 0);
	int GetW() const override;
	int GetH() const override;

	DVector2 GetBoxTopLeft() const;
	DRect GetTextViewport() const;
	wstring GetLeftText() const;
	wstring GetSelectedText() const;
	wstring GetRightText() const;
	int GetEditBegin() const;
	int GetEditEnd() const;
	bool HasSelection() const;
	int GetMaxCursorX() const;
	int GetCursorX(App& app) const;

	void SelectStart(App& app);
	void SelectEnd(App& app);

	void AddCharacters(App& app);
	void HandleLeft(App& app);
	void HandleRight(App& app);
	void HandleBackspace(App& app);
	void HandleDelete(App& app);
	void HandleCut(App& app);
	void HandleCopy(App& app);
	void HandlePaste(App& app);
	void HandleSelectAll(App& app);
	void HandleKey(App& app);

	void ClampSelectingCursor(App& app);
	void ClampCursor(App& app);
	void ClampCursorPosition(App& app);
	void ClampTextPosition(App& app);

	void RenderMain(App& app);
	void RenderCursor(App& app);
	void render(App& app);

	virtual void Sync(App& app) {}
	virtual void OnDone(App& app) const {}
	virtual void Upload(App& app) const {}

	void Update(App& app) override;
	void Discard(App& app) override;
	void PreUpdate(App& app) override;
};
struct LocalizedTextBox : TextBox {
	wstring id_txt;
	LocalizedTextBox() = default;
	LocalizedTextBox(wstring const& id, int w);
	void Update(App& app) override;
};


