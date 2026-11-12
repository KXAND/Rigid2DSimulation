#pragma once

#include <memory>

#include "Control.h"
#include "Tile.h"

class App;
class VerticalScrollBar;
class Panel
{
public:
    DVector2 topLeft;
    double depth = 0;
    int width = 0, height = 0;
    double wheelMultiplier = 0;
    DVector2 contentOffset;
    DRect contentViewport;
    Color frameColor, panelColor;

    bool useScrollBar = false;
    // TODO: 滚动条目前仅由面板持有，评估改为值成员或 unique_ptr。
    std::shared_ptr<VerticalScrollBar> scrollBar;

    int visibleHeight = 0;
    bool hasOverflow = false;
    bool hovered = false;
    bool wheeled = false;

    explicit Panel(DRect const& displayBounds);
    static Panel CreateMenuPanel(DRect const& displayBounds);
    static Panel CreateBottomPanel(DRect const& displayBounds);

    int GetMinY() const;
    DRect GetVisibleViewport() const;
    void SetDisplayBounds(DRect const& bounds);
    void Render(App& app);

    virtual void Update(App& app);
    // 如果有可删除的 Panel 就用这个。
    virtual void Discard(App& app);
    virtual void PreUpdate(App& app);

protected:
    // 构造期间安装一次根内容；不支持运行中替换，以免遗留输入归属。
    void SetContent(std::unique_ptr<Control> content);

private:
    void InitializeScrollBar();
    DRect displayBounds;
    std::unique_ptr<Control> contentControl;
};
