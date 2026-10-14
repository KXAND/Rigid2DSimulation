#pragma once
#include <functional>

#include "Tile.h"
#include "Control.h"

class HorizontalDragBar : public Control
{
public:
    DVector2 gap;
    double depth = 0;
    wstring text;
    int barWidth = 0, barHeight = 0;
    int nodeWidth = 0, nodeHeight = 0;
    Color textColor, barColor, normalColor, hoveredColor, invalidColor, draggedColor;

    int textWidth = 0;
    int nodeX = 0;
    bool dragged = false;
    bool enabled = true;
    bool hovered = false;

    HorizontalDragBar(int width = 0);
    int GetWidth() const override;
    int GetHeight() const override;

    double GetNormalizedValue() const;
    void SetNormalizedNodePosition(double normalizedValue);
    DVector2 GetNodeTopLeft() const;

    void Render(App& app);
    std::function<double(App&)> valueProvider;
    // 原拖动分支持续回写，包括释放鼠标的最后一帧。
    std::function<void(App&, double)> onDragging;

    virtual void Sync(App& app);
    virtual void OnDone(App& app) const {}
    virtual void Upload(App& app) const;

    void Update(App& app) override;
    void Discard(App& app) override;
    void PreUpdate(App& app) override;
};
class LocalizedHorizontalDragBar : public HorizontalDragBar
{
public:
    wstring textId;
    LocalizedHorizontalDragBar() = default;
    LocalizedHorizontalDragBar(wstring const& id, int width);
    void Update(App& app) override;
};
