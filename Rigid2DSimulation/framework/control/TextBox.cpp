#include "TextBox.h"
#include "App.h"
#include "DrawGeometry.h"
#include "DrawString.h"
#include "DrawBorderedRectangle.h"
#include "RichTextBox.h"

TextBox::TextBox(int width) : width(width)
{
    gap = {10, 10};
    depth = 1000;
    height = 30;

    labelTextColor = {255, 255, 255};
    valueColor = {255, 255, 255};
    editColor = {150, 60, 60};
    frameColor = {150, 150, 150};
    normalColor = {40, 40, 40};
    cursorColor = {255, 255, 255};
    hoveredColor = {60, 60, 150};
    invalidColor = {0, 0, 0};
    selectedColor = {150, 150, 255};

    editMarginWidth = 5;
    textViewportMarginWidth = 2;
    slideMultiplier = 8;
    blinkMultiplier = 8;
    cursorWidth = 2;
    cursorHeight = 20;
}
int TextBox::GetWidth() const
{
    return width + gap.x + labelTextWidth;
}
int TextBox::GetHeight() const
{
    return height + gap.y;
}

DVector2 TextBox::GetBoxTopLeft() const
{
    return topLeft + DVector2(labelTextWidth, 0);
}
DRect TextBox::GetTextViewport() const
{
    DVector2 textViewportTopLeft = GetBoxTopLeft() + DVector2(textViewportMarginWidth, 0);
    int textViewportWidth = width - 2 * textViewportMarginWidth;
    return GetOverlapRect(viewport, {textViewportTopLeft, textViewportWidth, height});
}
wstring TextBox::GetLeftText() const
{
    auto rangeBegin = value.begin();
    auto rangeEnd = value.begin() + GetEditBegin();
    return wstring(rangeBegin, rangeEnd);
}
wstring TextBox::GetSelectedText() const
{
    auto rangeBegin = value.begin() + GetEditBegin();
    auto rangeEnd = value.begin() + GetEditEnd();
    return wstring(rangeBegin, rangeEnd);
}
wstring TextBox::GetRightText() const
{
    auto rangeBegin = value.begin() + GetEditEnd();
    auto rangeEnd = value.end();
    return wstring(rangeBegin, rangeEnd);
}
int TextBox::GetEditBegin() const
{
    return min(selectionStart, selectionEnd);
}
int TextBox::GetEditEnd() const
{
    return max(selectionStart, selectionEnd);
}
bool TextBox::HasSelection() const
{
    return selectionStart != selectionEnd;
}
// 下面是对的，不需要加一减一什么的。
int TextBox::GetMaxCursorX() const
{
    return width - editMarginWidth;
}
int TextBox::GetCursorX(App& app) const
{
    wstring tmp(value.begin(), value.begin() + selectionEnd);
    return valueOffsetX + GetStringSize(tmp, app.font, 0).x;
}

void TextBox::SelectStart(App& app)
{
    selecting = true;
    int x = app.input.mousePosition.x - (GetBoxTopLeft().x + valueOffsetX);
    selectionStart = selectionEnd = GetStringIndexAtPosition(value, app.font, {x, 0}, 0);
}
void TextBox::SelectEnd(App& app)
{
    int x = app.input.mousePosition.x - (GetBoxTopLeft().x + valueOffsetX);
    selectionEnd = GetStringIndexAtPosition(value, app.font, {x, 0}, 0);
}

void TextBox::AddCharacters(App& app)
{
    wstring sifted = FilterCharacters(app.input.typedCharacters, true);
    if (!sifted.empty())
    {
        selecting = false;
        value = GetLeftText() + sifted + GetRightText();
        selectionStart = selectionEnd = GetEditBegin() + sifted.size();
    }
}
void TextBox::HandleLeft(App& app)
{
    selecting = false;
    isCursorChanged = true;

    if (HasSelection())
    {
        selectionStart = selectionEnd = GetEditBegin();
    }
    else
    {
        if (--selectionStart < 0)
        {
            selectionStart = 0;
        }
        selectionEnd = selectionStart;
    }
}
void TextBox::HandleRight(App& app)
{
    selecting = false;
    isCursorChanged = true;

    if (HasSelection())
    {
        selectionStart = selectionEnd = GetEditEnd();
    }
    else
    {
        // 下面不要减一。
        if (++selectionStart > value.size())
        {
            selectionStart = value.size();
        }
        selectionEnd = selectionStart;
    }
}
void TextBox::HandleBackspace(App& app)
{
    selecting = false;
    isCursorChanged = true;

    if (HasSelection())
    {
        value = GetLeftText() + GetRightText();
        selectionStart = selectionEnd = GetEditBegin();
    }
    else if (GetEditBegin() > 0)
    {
        valueOffsetX += app.font.GetGlyph(value[selectionStart - 1]).advance;
        valueOffsetX = min<double>(valueOffsetX, editMarginWidth);
        auto rangeBegin = value.begin();
        auto rangeEnd = value.begin() + selectionStart - 1;
        value = wstring(rangeBegin, rangeEnd) + GetRightText();
        selectionEnd = --selectionStart;
    }
}
void TextBox::HandleDelete(App& app)
{
    selecting = false;
    isCursorChanged = true;

    if (HasSelection())
    {
        value = GetLeftText() + GetRightText();
        selectionStart = selectionEnd = GetEditBegin();
    }
    else if (GetEditEnd() < value.size())
    {
        auto rangeBegin = value.begin() + selectionStart + 1;
        auto rangeEnd = value.end();
        value = GetLeftText() + wstring(rangeBegin, rangeEnd);
    }
}
void TextBox::HandleCut(App& app)
{
    selecting = false;
    SetClipboard(GetSelectedText());
    value = GetLeftText() + GetRightText();
    selectionStart = selectionEnd = GetEditBegin();
}
void TextBox::HandleCopy(App& app)
{
    SetClipboard(GetSelectedText());
}
void TextBox::HandlePaste(App& app)
{
    selecting = false;
    wstring sifted = FilterCharacters(GetClipboard(), true);
    value = GetLeftText() + sifted + GetRightText();
    selectionStart = selectionEnd = GetEditBegin() + sifted.size();
}
void TextBox::HandleSelectAll(App& app)
{
    selecting = false;
    selectionStart = 0;
    selectionEnd = value.size();
}
void TextBox::HandleKey(App& app)
{
    isCursorChanged = false;

    if (app.input.keyMessageCounts[VK_LEFT])
    {
        HandleLeft(app);
    }
    else if (app.input.keyMessageCounts[VK_RIGHT])
    {
        HandleRight(app);
    }
    else if (app.input.keyMessageCounts[VK_DELETE])
    {
        HandleDelete(app);
    }
    else if (app.input.keyMessageCounts[VK_BACK])
    {
        HandleBackspace(app);
    }

    if (app.input.keyDown[VK_CONTROL])
    {
        if (app.input.IsKeyPressedDown('C'))
        {
            HandleCopy(app);
        }
        else if (app.input.IsKeyPressedDown('X'))
        {
            HandleCut(app);
        }
        else if (app.input.IsKeyPressedDown('V'))
        {
            HandlePaste(app);
        }
        else if (app.input.IsKeyPressedDown('A'))
        {
            HandleSelectAll(app);
        }
    }
}

void TextBox::ClampSelectingCursor(App& app)
{
    double cursorOverflow = (app.input.mousePosition.x - GetBoxTopLeft().x) - GetMaxCursorX();
    if (cursorOverflow > 0)
    {
        valueOffsetX -= app.frameRate.GetDeltaTime() * slideMultiplier * cursorOverflow;
    }
    cursorOverflow = editMarginWidth - (app.input.mousePosition.x - GetBoxTopLeft().x);
    if (cursorOverflow > 0)
    {
        valueOffsetX += app.frameRate.GetDeltaTime() * slideMultiplier * cursorOverflow;
    }
}
void TextBox::ClampCursor(App& app)
{
    if (GetCursorX(app) > GetMaxCursorX())
    {
        valueOffsetX -= GetCursorX(app) - GetMaxCursorX();
    }
    else if (GetCursorX(app) < editMarginWidth)
    {
        valueOffsetX += editMarginWidth - GetCursorX(app);
    }
}
void TextBox::ClampCursorPosition(App& app)
{
    if (selecting)
    {
        ClampSelectingCursor(app);
    }
    else
    {
        ClampCursor(app);
    }
}
void TextBox::ClampTextPosition(App& app)
{
    int valueEndX = valueOffsetX + GetStringSize(value, app.font, 0).x;
    if (valueEndX < GetMaxCursorX())
    {
        valueOffsetX += GetMaxCursorX() - valueEndX;
    }
    if (valueOffsetX > editMarginWidth)
    {
        valueOffsetX = editMarginWidth;
    }
}

void TextBox::RenderMain(App& app)
{
    DVector2 labelTextTopLeft = topLeft + DVector2(0, (height - app.font.height) / 2);
    DrawString(app.screen, app.depthBuffer, depth, labelText, labelTextColor, app.font, labelTextTopLeft, 0, viewport);

    Color const& color =
        !enabled ? invalidColor : edit                                                                                                       ? editColor
                              : (hovered && !app.input.mouseButtonDown[0] && !app.input.mouseButtonDown[1] && !app.input.mouseButtonDown[2]) ? hoveredColor
                                                                                                                                             : normalColor;
    DrawRectangleWithBorderRaw(app.screen, GetBoxTopLeft(), width, height, viewport, color, frameColor);

    int cursorX = 0;
    DVector2 valueTopLeft = GetBoxTopLeft() + DVector2(valueOffsetX, (height - app.font.height) / 2);
    DrawString(app.screen, app.depthBuffer, depth, GetLeftText(), valueColor, app.font, valueTopLeft, cursorX, 0, GetTextViewport());
    DrawString(app.screen, app.depthBuffer, depth, GetSelectedText(), selectedColor, app.font, valueTopLeft, cursorX, 0, GetTextViewport());
    DrawString(app.screen, app.depthBuffer, depth, GetRightText(), valueColor, app.font, valueTopLeft, cursorX, 0, GetTextViewport());
}
void TextBox::RenderCursor(App& app)
{
    bool isCursorVisible = edit &&
                           (sin(app.frameRate.GetElapsedTime() * blinkMultiplier) > 0 || isCursorChanged);
    if (isCursorVisible)
    {
        int cursorScreenX = GetBoxTopLeft().x + GetCursorX(app) - cursorWidth / 2;
        int cursorScreenY = GetBoxTopLeft().y + (height - cursorHeight) / 2;
        DrawRectangleRaw(app.screen, {cursorScreenX, cursorScreenY}, cursorWidth, cursorHeight, GetTextViewport(), cursorColor);
    }
}
void TextBox::Render(App& app)
{
    RenderMain(app);
    RenderCursor(app);
}

void TextBox::Sync(App& app)
{
    if (valueProvider)
    {
        value = valueProvider(app);
    }
}

void TextBox::OnDone(App& app) const
{
    if (onEditFinished)
    {
        onEditFinished(app, value);
    }
}

void TextBox::Upload(App& app) const
{
    if (onEditing)
    {
        onEditing(app, value);
    }
}

void TextBox::Update(App& app)
{
    hovered = (app.inputOwners.hovered == this);
    labelTextWidth = GetStringSize(labelText, app.font, 0).x;
    if (edit)
    {
        if (selecting)
        {
            SelectEnd(app);
        }
        if (enabled && hovered && app.input.IsMouseClickedDown(0))
        {
            SelectStart(app);
        }
        if (!app.input.mouseButtonDown[0])
        {
            selecting = false;
        }
        HandleKey(app);
        AddCharacters(app);
        ClampCursorPosition(app);
        Upload(app);

        if (enabled && !hovered && app.input.IsMouseClickedDown(0))
        {
            OnDone(app);
        }
        if (!enabled || !hovered && app.input.IsMouseClickedDown(0))
        {
            // 请注意 kb_owner 的登记在 PreUpdate 中进行。
            app.inputOwners.ReleaseKeyboard(this);
            edit = selecting = false;
            selectionStart = selectionEnd = 0;
        }
    }
    else
    {
        Sync(app);
        if (enabled && hovered && app.input.IsMouseClickedDown(0))
        {
            if (clearUponEdit)
            {
                value.clear();
            }
            SelectStart(app);
            edit = true;
        }
    }
    ClampTextPosition(app);

    // 提交和释放焦点属于更新阶段；渲染读取提交后的状态。
    if (submitOnEnter && edit && app.input.IsKeyPressedDown(VK_RETURN))
    {
        OnDone(app);
        Discard(app);
    }
}
void TextBox::Discard(App& app)
{
    app.inputOwners.Remove(this);
    app.inputOwners.ReleaseKeyboard(this);
    labelTextWidth = 0;
    edit = hovered = false;
    selecting = isCursorChanged = false;
    selectionStart = selectionEnd = 0;
}
void TextBox::PreUpdate(App& app)
{
    if (edit)
    {
        app.inputOwners.keyboardOwner = this;
    }

    bool canReceivePointer = app.inputOwners.hoveredDepth <= depth &&
                             IsInside(app.input.mousePosition, {GetBoxTopLeft(), width, height}) && IsInside(app.input.mousePosition, viewport);
    if (canReceivePointer)
    {
        app.inputOwners.hoveredDepth = depth;
        app.inputOwners.hovered = this;
    }
}

LocalizedTextBox::LocalizedTextBox(wstring const& id, int width) : TextBox(width), labelTextId(id) {}
void LocalizedTextBox::Update(App& app)
{
    labelText = app.GetLocalizedString(labelTextId);
    TextBox::Update(app);
}
