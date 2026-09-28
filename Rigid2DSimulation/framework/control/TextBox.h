#pragma once
#include "Font.h"
#include "Control.h"

struct TextBox : Control
{
    DVector2 gap;
    double depth = 0;
    wstring labelText;
    int width = 0, height = 0;
    bool clearUponEdit = false;
    Color labelTextColor, valueColor, editColor, frameColor, normalColor,
        cursorColor, hoveredColor, invalidColor, selectedColor;

    // cs = cursor，mg = margin。
    // 也是 valueOffsetX 的默认值，否则一开始编辑文字的位置就会变化。
    int editMarginWidth = 0;
    int textViewportMarginWidth = 0;
    double slideMultiplier = 0;
    double blinkMultiplier = 0;
    int cursorWidth = 0, cursorHeight = 0;

    wstring value;
    double valueOffsetX = 0;
    int selectionStart = 0, selectionEnd = 0;

    int labelTextWidth = 0;
    bool edit = false;
    bool enabled = true;
    bool hovered = false;
    bool selecting = false;
    bool isCursorChanged = false;

    TextBox(int width = 0);
    int GetWidth() const override;
    int GetHeight() const override;

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
    void Render(App& app);

    virtual void Sync(App& app) {}
    virtual void OnDone(App& app) const {}
    virtual void Upload(App& app) const {}

    void Update(App& app) override;
    void Discard(App& app) override;
    void PreUpdate(App& app) override;
};
struct LocalizedTextBox : TextBox
{
    wstring labelTextId;
    LocalizedTextBox() = default;
    LocalizedTextBox(wstring const& id, int width);
    void Update(App& app) override;
};
