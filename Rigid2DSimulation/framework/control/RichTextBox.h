#pragma once
#include "Vector2.h"
#include "Font.h"
#include "Control.h"

class RichTextBox : public Control
{
public:
    DVector2 gap;
    int lineWidth = 0;
    double depth = 0;
    int width = 0, height = 0;
    bool clearUponEdit = false;
    Color valueColor, editColor, frameColor, normalColor,
        cursorColor, hoveredColor, invalidColor, selectedColor;

    DVector2 editMargin;
    DVector2 textViewportMargin;
    double slideMultiplier = 0;
    double wheelMultiplier = 0;
    double blinkMultiplier = 0;
    int cursorWidth = 0, cursorHeight = 0;

    wstring value;
    Vector2 valueOffset;
    int selectionStart = 0, selectionEnd = 0;
    // 下面的 rel 是相对于 valueOffset 的。
    int cursorXHistory = 0;

    bool edit = false;
    bool enabled = true;
    bool hovered = false;
    bool selecting = false;
    bool cursorChanged = false;

    RichTextBox();
    RichTextBox(int width, int height, bool newline = true);
    int GetWidth() const override;
    int GetHeight() const override;

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
