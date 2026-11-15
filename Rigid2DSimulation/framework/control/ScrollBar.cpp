#include "ScrollBar.h"
#include "App.h"
#include "Control.h"
#include "DrawBorderedRectangle.h"

VerticalScrollBar::VerticalScrollBar()
{
    depth = 1000;
    width = 20;

    barColor = {0, 0, 0};
    frameColor = {150, 150, 150};
    normalColor = {80, 80, 80};
    draggedColor = {150, 60, 60};
    hoveredColor = {60, 60, 150};
    invalidColor = {0, 0, 0};
}
void VerticalScrollBar::Render(App& app)
{
    DrawRectangleWithBorderRaw(app.screen, topLeft, width, height, app.screen.Rect(), barColor, frameColor);
    if (hasOverflow)
    {
        Color const& color =
            !enabled ? invalidColor : dragged                                                                                                    ? draggedColor
                                  : (hovered && !app.input.mouseButtonDown[0] && !app.input.mouseButtonDown[1] && !app.input.mouseButtonDown[2]) ? hoveredColor
                                                                                                                                                 : normalColor;
        DrawRectangleWithBorderRaw(app.screen, topLeft + DVector2(0, thumbY), width, thumbHeight, app.screen.Rect(), color, frameColor);
    }
}

void VerticalScrollBar::Update(App& app)
{
    hasOverflow = (contentControl != NULL) && visibleHeight < contentControl->GetHeight();
    hovered = (app.inputOwners.hovered == this);
    hoveredThumb = hovered && IsInside(app.input.mousePosition, {topLeft + DVector2(0, thumbY), width, thumbHeight});

    // 这个状态变化比一般的控件是要复杂的。
    if (hasOverflow)
    {
        thumbHeight = visibleHeight * height / contentControl->GetHeight();
        if (dragged)
        {
            thumbY += app.input.mousePosition.y - app.input.previousMousePosition.y;
            thumbY = Clamp(thumbY, 0, height - thumbHeight);
            contentControl->topLeft.y = visibleTop - thumbY * contentControl->GetHeight() / height;
            dragged = hasOverflow && enabled && app.input.mouseButtonDown[0];
        }
        else
        {
            thumbY = (visibleTop - contentControl->topLeft.y) * height / contentControl->GetHeight();
            thumbY = Clamp(thumbY, 0, height - thumbHeight);
            dragged = hasOverflow && enabled && hovered && app.input.IsMouseClickedDown(0);
        }
    }
    else
    {
        dragged = false;
    }
}
void VerticalScrollBar::Discard(App& app)
{
    app.inputOwners.Remove(this);
    dragged = hovered = hasOverflow = hoveredThumb = false;
}
void VerticalScrollBar::PreUpdate(App& app)
{
    bool canReceivePointer = app.inputOwners.hoveredDepth <= depth &&
                             IsInside(app.input.mousePosition, {topLeft, width, height}) && IsInside(app.input.mousePosition, app.screen.Rect());
    if (canReceivePointer)
    {
        app.inputOwners.hoveredDepth = depth;
        app.inputOwners.hovered = this;
    }
}
