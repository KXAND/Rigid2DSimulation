#include "Panel.h"

#include <stdexcept>
#include <utility>

#include "App.h"
#include "Control.h"
#include "DrawBorderedRectangle.h"
#include "ScrollBar.h"

Panel::Panel(DRect const& displayBounds) : displayBounds(displayBounds)
{
    depth = 1000;
    wheelMultiplier = 40;
    contentOffset = {10, 10};

    frameColor = {150, 150, 150};
    panelColor = {0, 0, 0};
    scrollBar = std::make_shared<VerticalScrollBar>();
}
// 菜单按钮的宽度: 150.
Panel Panel::CreateMenuPanel(DRect const& displayBounds)
{
    Panel panel(displayBounds);
    panel.topLeft = displayBounds.topLeftPosition;
    panel.width = 169;
    panel.height = displayBounds.height - 40;
    panel.visibleHeight = panel.height - 20;

    DRect contentViewportRelative;
    contentViewportRelative.topLeftPosition = DVector2(10 - 2, 10 - 2);
    contentViewportRelative.width = panel.width - 20 + 4;
    contentViewportRelative.height = panel.height - 20 + 4;
    panel.contentViewport = contentViewportRelative;
    panel.InitializeScrollBar();
    return panel;
}
Panel Panel::CreateBottomPanel(DRect const& displayBounds)
{
    Panel panel(displayBounds);
    panel.topLeft = displayBounds.topLeftPosition;
    panel.width = displayBounds.width;
    panel.height = 40;
    panel.topLeft.y += displayBounds.height - panel.height;
    panel.visibleHeight = panel.height;

    DRect contentViewportRelative;
    contentViewportRelative.topLeftPosition = DVector2(10 - 2, 10 - 2);
    contentViewportRelative.width = panel.width - 20 + 4;
    contentViewportRelative.height = panel.height - 20 + 4;
    panel.contentViewport = contentViewportRelative;
    panel.InitializeScrollBar();
    return panel;
}

int Panel::GetMinY() const
{
    return topLeft.y + contentOffset.y + visibleHeight - contentControl->GetHeight();
}
DRect Panel::GetVisibleViewport() const
{
    DRect visibleViewport = contentViewport;
    visibleViewport.topLeftPosition += topLeft;
    return GetOverlapRect(displayBounds, visibleViewport);
}
void Panel::SetDisplayBounds(DRect const& bounds)
{
    displayBounds = bounds;
    if (contentControl)
    {
        contentControl->viewport = GetVisibleViewport();
    }
}

void Panel::SetContent(std::unique_ptr<Control> content)
{
    if (!content)
    {
        throw std::invalid_argument("Panel::SetContent requires content");
    }
    if (contentControl)
    {
        throw std::logic_error("Panel content is already installed");
    }

    contentControl = std::move(content);
    contentControl->topLeft = topLeft + contentOffset;
    contentControl->viewport = GetVisibleViewport();
    // 滚动条只借用内容；根控件的唯一所有者是面板。
    scrollBar->contentControl = contentControl.get();
}

void Panel::InitializeScrollBar()
{
    scrollBar->contentControl = contentControl.get();
    scrollBar->height = height;
    scrollBar->topLeft = topLeft + DVector2(width - scrollBar->width, 0);
    scrollBar->visibleHeight = visibleHeight;
    scrollBar->visibleTop = topLeft.y + contentOffset.y;
}
void Panel::Render(App& app)
{
    DrawRectangleWithBorderRaw(app.screen, topLeft, width, height, app.screen.Rect(), panelColor, frameColor);
}

void Panel::Update(App& app)
{
    hasOverflow = (contentControl != nullptr) && visibleHeight < contentControl->GetHeight();
    hovered = (app.inputOwners.hovered == this);
    wheeled = (app.inputOwners.wheeled == this);

    if (hasOverflow)
    {
        int contentY = contentControl->topLeft.y;
        if (wheeled)
        {
            contentY += app.input.mouseWheelDelta * wheelMultiplier;
        }
        contentControl->topLeft.y = Clamp(contentY, GetMinY(), topLeft.y + contentOffset.y);
    }
    else if (contentControl)
    {
        contentControl->topLeft.y = topLeft.y + contentOffset.y;
    }

    // 下面 Update 顺序是为了方便绘制，本来应该控件先更新高度的。
    // 下面 Update 顺序是为了方便绘制，本来应该控件先更新高度的。
    // 这个是为了速度的牺牲，不知道有没有必要。
    Render(app);
    if (contentControl)
    {
        contentControl->Update(app);
    }
    if (useScrollBar)
    {
        scrollBar->Update(app);
    }
}
void Panel::Discard(App& app)
{
    app.inputOwners.Remove(this);
    scrollBar->Discard(app);
    if (contentControl)
    {
        contentControl->Discard(app);
    }
}
void Panel::PreUpdate(App& app)
{
    bool canOwnInput = app.inputOwners.hoveredDepth <= depth &&
                       IsInside(app.input.mousePosition, {topLeft, width, height}) && IsInside(app.input.mousePosition, app.screen.Rect());
    if (canOwnInput)
    {
        app.inputOwners.hoveredDepth = depth;
        app.inputOwners.hovered = this;
    }

    canOwnInput = app.inputOwners.wheeledDepth <= depth &&
                  IsInside(app.input.mousePosition, {topLeft, width, height}) && IsInside(app.input.mousePosition, app.screen.Rect());
    if (canOwnInput)
    {
        app.inputOwners.wheeledDepth = depth;
        app.inputOwners.wheeled = this;
    }

    if (contentControl)
    {
        contentControl->PreUpdate(app);
    }
    if (useScrollBar)
    {
        scrollBar->PreUpdate(app);
    }
}
