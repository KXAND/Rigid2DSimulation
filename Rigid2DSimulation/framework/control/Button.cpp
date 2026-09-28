#include "Button.h"
#include "App.h"
#include "DrawString.h"
#include "DrawBorderedRectangle.h"
#include "FloatPanel.h"

Button::Button(int width) : width(width)
{
    gap = {10, 10};
    depth = 1000;
    text = L"按钮";
    height = 30;

    textColor = {255, 255, 255};
    frameColor = {150, 150, 150};
    normalColor = {80, 80, 80};
    chosenColor = {150, 60, 60};
    hoveredColor = {60, 60, 150};
    invalidColor = {0, 0, 0};
}
int Button::GetWidth() const
{
    return width + gap.x;
}
int Button::GetHeight() const
{
    return height + gap.y;
}

void Button::Render(App& app)
{
    Color const& color =
        !enabled ? invalidColor : chosen                                                                                                     ? chosenColor
                              : (hovered && !app.input.mouseButtonDown[0] && !app.input.mouseButtonDown[1] && !app.input.mouseButtonDown[2]) ? hoveredColor
                                                                                                                                             : normalColor;
    DVector2 textSize = GetStringSize(text, app.font, 0);
    DVector2 textTopLeft = topLeft + DVector2(width, height) / 2 - textSize / 2;

    DrawRectangleWithBorderRaw(app.screen, topLeft, width, height, viewport, color, frameColor);
    DrawString(app.screen, app.depthBuffer, depth, text, textColor, app.font, textTopLeft, 0, viewport);
}

void Button::Update(App& app)
{
    hovered = (app.inputOwners.hovered == this);
    if (chosen)
    {
        if (enabled && !app.input.mouseButtonDown[0])
        {
            OnClick(app);
        }
        chosen = enabled && hovered && app.input.mouseButtonDown[0];
    }
    else
    {
        chosen = enabled && hovered && app.input.IsMouseClickedDown(0);
    }
    Render(app);
}
void Button::Discard(App& app)
{
    app.inputOwners.Remove(this);
    chosen = hovered = false;
}
void Button::PreUpdate(App& app)
{
    bool canReceivePointer = app.inputOwners.hoveredDepth <= depth &&
                             IsInside(app.input.mousePosition, {topLeft, width, height}) && IsInside(app.input.mousePosition, viewport);
    if (canReceivePointer)
    {
        app.inputOwners.hoveredDepth = depth;
        app.inputOwners.hovered = this;
    }
}

LocalizedButton::LocalizedButton(int width, wstring const& id) : Button(width), textId(id) {}
void LocalizedButton::Update(App& app)
{
    text = app.GetLocalizedString(textId);
    Button::Update(app);
}

ShowFloatPanelButton::ShowFloatPanelButton(FloatPanel* floatPanel) : Button(150), floatPanel(floatPanel) {}
void ShowFloatPanelButton::OnClick(App& app)
{
    floatPanel->Show(app);
}
void ShowFloatPanelButton::Update(App& app)
{
    enabled = !floatPanel->shown;
    text = floatPanel->title;
    Button::Update(app);
}
