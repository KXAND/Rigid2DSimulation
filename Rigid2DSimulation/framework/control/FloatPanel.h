#pragma once

#include <memory>

#include "Control.h"
#include "Tile.h"

class App;
class VerticalScrollBar;
class FloatPanel
{
public:
    DVector2 topLeft;
    double depth = 0;
    wstring title;
    int width = 0, height = 0;
    double wheelMultiplier = 0;
    DVector2 contentOffset;
    DRect contentViewport;

    bool useScrollBar = false;
    // TODO: 滚动条目前仅由面板持有，评估改为值成员或 unique_ptr。
    std::shared_ptr<VerticalScrollBar> scrollBar;

    int titleBarHeight = 0;
    int closeButtonSize = 0;
    int closeMarkHalfSize = 0;
    int closeButtonMargin = 0;
    int titleMargin = 0;
    Color titleColor, closeMarkColor, frameColor, panelColor;
    Color titleBarNormalColor, titleBarDraggedColor, titleBarHoveredColor;
    Color closeButtonNormalColor, closeButtonChosenColor, closeButtonHoveredColor, closeButtonInvalidColor;

    int visibleHeight = 0;
    double lastClickTime = 0;
    bool hasOverflow = false;
    bool shown = false;
    bool hovered = false;
    bool wheeled = false;
    bool titleBarHovered = false;
    bool titleBarDragged = false;
    bool closeButtonChosen = false;
    bool closeButtonHovered = false;
    bool closeButtonEnabled = true;

    FloatPanel(DRect const& displayBounds, int width, int height);

    int GetMinY() const;
    DRect GetVisibleViewport() const;
    void SetDisplayBounds(DRect const& bounds);
    DVector2 GetCloseTopLeft() const;
    void SetTopLeft(DVector2 const& topLeft);

    void Hide(App& app);
    void Show(App& app);
    void HandleCaption(App& app);
    void HandleWheelAndClampContent(App& app);

    void RenderMain(App& app);
    void RenderClose(App& app);
    void Render(App& app);

    virtual void Update(App& app);
    virtual void Discard(App& app);
    virtual void PreUpdate(App& app);

protected:
    // 构造期间安装一次根内容；不支持运行中替换，以免遗留输入归属。
    void SetContent(std::unique_ptr<Control> content);

    // 仅在仍显示的面板中调用，位于滚动/标题处理之后、内容遍历的位置。
    virtual void UpdateContent(App& app);

private:
    FloatPanel();
    void InitializeScrollBar();
    DRect displayBounds;
    std::unique_ptr<Control> contentControl;
};
class LocalizedFloatPanel : public FloatPanel
{
public:
    wstring titleLocalizationId;
    LocalizedFloatPanel(DRect const& displayBounds, int width, int height, wstring const& id);
    void Update(App& app) override;
};

bool CompareFloatPanels(FloatPanel* const firstPanel, FloatPanel* const secondPanel);
