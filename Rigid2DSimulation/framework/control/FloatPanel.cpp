#include "FloatPanel.h"

#include <stdexcept>
#include <utility>

#include "App.h"
#include "Control.h"
#include "ScrollBar.h"
#include "DrawString.h"
#include "DrawGeometry.h"
#include "DrawBorderedRectangle.h"
#include "DrawLineSegment.h"

FloatPanel::FloatPanel()
{
    depth = 1000;
    title = L"窗口";
    wheelMultiplier = 40;
    contentOffset = {10, 40};

    titleBarHeight = 30;
    closeButtonSize = 20;
    closeMarkHalfSize = 5;
    closeButtonMargin = 5;
    titleMargin = 5;

    titleColor = {255, 255, 255};
    closeMarkColor = {255, 255, 255};
    frameColor = {150, 150, 150};
    panelColor = {0, 0, 0};

    titleBarNormalColor = {40, 40, 40};
    titleBarDraggedColor = {80, 40, 40};
    titleBarHoveredColor = {40, 40, 80};

    closeButtonNormalColor = {0, 0, 0};
    closeButtonChosenColor = {255, 20, 20};
    closeButtonHoveredColor = {20, 20, 255};
    closeButtonInvalidColor = {255, 255, 255};
}
FloatPanel::FloatPanel(DRect const& displayBounds, int width, int height) : FloatPanel()
{
    this->displayBounds = displayBounds;
    this->width = width;
    this->height = height;
    topLeft = displayBounds.topLeftPosition + DVector2(displayBounds.width - width, displayBounds.height - height) / 2;
    visibleHeight = height - 50;
    useScrollBar = true;

    DRect contentViewportRelative;
    contentViewportRelative.topLeftPosition = DVector2(10 - 2, 40 - 2);
    contentViewportRelative.width = width - 40 + 4;
    contentViewportRelative.height = height - 50 + 4;
    contentViewport = contentViewportRelative;
    InitializeScrollBar();
}

int FloatPanel::GetMinY() const
{
    return topLeft.y + contentOffset.y + visibleHeight - contentControl->GetHeight();
}
DRect FloatPanel::GetVisibleViewport() const
{
    DRect visibleViewport = contentViewport;
    visibleViewport.topLeftPosition += topLeft;
    return GetOverlapRect(displayBounds, visibleViewport);
}
DVector2 FloatPanel::GetCloseTopLeft() const
{
    int closeX = width - closeButtonMargin - closeButtonSize;
    int closeY = (titleBarHeight - closeButtonSize) / 2;
    return topLeft + DVector2(closeX, closeY);
}
void FloatPanel::SetDisplayBounds(DRect const& bounds)
{
    displayBounds = bounds;
    if (contentControl)
    {
        contentControl->viewport = GetVisibleViewport();
    }
}

void FloatPanel::SetContent(std::unique_ptr<Control> content)
{
    if (!content)
    {
        throw std::invalid_argument("FloatPanel::SetContent requires content");
    }
    if (contentControl)
    {
        throw std::logic_error("FloatPanel content is already installed");
    }

    contentControl = std::move(content);
    contentControl->topLeft = topLeft + contentOffset;
    contentControl->viewport = GetVisibleViewport();
    // 滚动条只借用内容；根控件的唯一所有者是面板。
    scrollBar->contentControl = contentControl.get();
}

void FloatPanel::SetTopLeft(DVector2 const& topLeft)
{
    if (contentControl)
    {
        contentControl->topLeft += topLeft - this->topLeft;
        // 在调用 GetVisibleViewport 之前需要先更新 topLeft。
        this->topLeft = topLeft;
        contentControl->viewport = GetVisibleViewport();
    }
    scrollBar->topLeft = topLeft + DVector2(width - scrollBar->width, titleBarHeight);
    scrollBar->visibleTop = topLeft.y + contentOffset.y;
}
void FloatPanel::InitializeScrollBar()
{
    scrollBar = std::make_shared<VerticalScrollBar>();
    scrollBar->contentControl = contentControl.get();
    scrollBar->height = height - titleBarHeight;
    scrollBar->topLeft = topLeft + DVector2(width - scrollBar->width, titleBarHeight);
    scrollBar->visibleHeight = visibleHeight;
    scrollBar->visibleTop = topLeft.y + contentOffset.y;
}

void FloatPanel::Hide(App& app)
{
    shown = false;
    Discard(app);
}
void FloatPanel::Show(App& app)
{
    shown = true;
    lastClickTime = app.frameRate.GetElapsedTime();
}
void FloatPanel::HandleCaption(App& app)
{
    closeButtonHovered = hovered && IsInside(app.input.mousePosition, {GetCloseTopLeft(), closeButtonSize, closeButtonSize});
    titleBarHovered = hovered && !closeButtonHovered && IsInside(app.input.mousePosition, {topLeft, width, titleBarHeight});

    if (titleBarDragged)
    {
        DVector2 newTopLeft = topLeft + app.input.mousePosition - app.input.previousMousePosition;
        newTopLeft.x = Clamp(newTopLeft.x, 0, app.screen.width - width);
        newTopLeft.y = Clamp(newTopLeft.y, 0, app.screen.height - titleBarHeight);
        SetTopLeft(newTopLeft);
        titleBarDragged = app.input.mouseButtonDown[0];
    }
    else
    {
        titleBarDragged = titleBarHovered && app.input.IsMouseClickedDown(0);
    }

    if (closeButtonChosen)
    {
        if (closeButtonEnabled && !app.input.mouseButtonDown[0])
        {
            Hide(app);
            return;
        }
        closeButtonChosen = closeButtonEnabled && closeButtonHovered && app.input.mouseButtonDown[0];
    }
    else
    {
        closeButtonChosen = closeButtonEnabled && closeButtonHovered && app.input.IsMouseClickedDown(0);
    }
}
void FloatPanel::HandleWheelAndClampContent(App& app)
{
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
}

void FloatPanel::RenderMain(App& app)
{
    DrawRectangleWithBorderRaw(app.screen, topLeft, width, height, app.screen.Rect(), panelColor, frameColor);
    Color const& titleBarColor =
        titleBarDragged ? titleBarDraggedColor : titleBarHovered && (!app.input.mouseButtonDown[0] && !app.input.mouseButtonDown[1] && !app.input.mouseButtonDown[2]) ? titleBarHoveredColor
                                                                                                                                                                      : titleBarNormalColor;
    DrawRectangleWithBorderRaw(app.screen, topLeft, width, titleBarHeight, app.screen.Rect(), titleBarColor, frameColor);

    DVector2 titleTopLeft = topLeft + DVector2(titleMargin, (titleBarHeight - app.font.height) / 2);
    DrawString(app.screen, app.depthBuffer, depth, title, titleColor, app.font, titleTopLeft, 0, app.screen.Rect());
}
void FloatPanel::RenderClose(App& app)
{
    Color const& closeButtonColor =
        !closeButtonEnabled ? closeButtonInvalidColor : closeButtonChosen                                                                                                     ? closeButtonChosenColor
                                                    : closeButtonHovered && (!app.input.mouseButtonDown[0] && !app.input.mouseButtonDown[1] && !app.input.mouseButtonDown[2]) ? closeButtonHoveredColor
                                                                                                                                                                              : closeButtonNormalColor;
    DrawRectangleRaw(app.screen, GetCloseTopLeft(), closeButtonSize, closeButtonSize, app.screen.Rect(), closeButtonColor);

    DVector2 closeMarkCenter = GetCloseTopLeft() + DVector2(closeButtonSize, closeButtonSize) / 2;
    DVector2 closeMarkArm = {closeMarkHalfSize, closeMarkHalfSize};
    DrawLineSegment(app.screen, app.depthBuffer, closeMarkCenter - closeMarkArm, closeMarkCenter + closeMarkArm, depth, app.screen.Rect(), closeMarkColor);
    closeMarkArm = {-closeMarkHalfSize, closeMarkHalfSize};
    DrawLineSegment(app.screen, app.depthBuffer, closeMarkCenter - closeMarkArm, closeMarkCenter + closeMarkArm, depth, app.screen.Rect(), closeMarkColor);
}
void FloatPanel::Render(App& app)
{
    RenderMain(app);
    RenderClose(app);
}

// 这也是比较特殊，相当于要处理自己的 Discard。
void FloatPanel::Update(App& app)
{
    if (!shown)
    {
        return;
    }
    hasOverflow = (contentControl != nullptr) && visibleHeight < contentControl->GetHeight();
    hovered = (app.inputOwners.hovered == this);
    wheeled = (app.inputOwners.wheeled == this);

    HandleWheelAndClampContent(app);
    if (hovered && app.input.IsMouseClickedDown(0))
    {
        lastClickTime = app.frameRate.GetElapsedTime();
    }
    // 这项摆在最后是因为他可能执行 hide 指令。
    HandleCaption(app);

    // 我觉得这里乱掉了。但是为了这个重构好像不值得。
    if (shown)
    {
        Render(app);
        if (contentControl)
        {
            UpdateContent(app);
        }
        if (useScrollBar)
        {
            scrollBar->Update(app);
        }
    }
}
void FloatPanel::UpdateContent(App& app)
{
    contentControl->Update(app);
}

void FloatPanel::Discard(App& app)
{
    closeButtonChosen = closeButtonHovered =
        hovered = wheeled = titleBarHovered = titleBarDragged = false;
    app.inputOwners.Remove(this);
    scrollBar->Discard(app);
    if (contentControl)
    {
        contentControl->Discard(app);
    }
}
void FloatPanel::PreUpdate(App& app)
{
    if (!shown)
        return;
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

LocalizedFloatPanel::LocalizedFloatPanel(DRect const& displayBounds, int width, int height, wstring const& id)
    : FloatPanel(displayBounds, width, height), titleLocalizationId(id) {}
void LocalizedFloatPanel::Update(App& app)
{
    title = app.GetLocalizedString(titleLocalizationId);
    FloatPanel::Update(app);
}

bool CompareFloatPanels(FloatPanel* const firstPanel, FloatPanel* const secondPanel)
{
    return firstPanel->lastClickTime < secondPanel->lastClickTime;
}
