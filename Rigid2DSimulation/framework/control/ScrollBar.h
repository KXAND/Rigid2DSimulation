#pragma once
#include "Tile.h"

struct App;
struct Control;
// 如果对象的高度超过了范围，Scrollbar 不会自动更正。
// 这一权限留给 Panel 等对象的直接上级。
// Scrollbar 不是控件，不继承 Control 类。
struct VerticalScrollBar
{
    DVector2 topLeft;
    double depth = 0;
    Control* contentControl = NULL;
    int width = 0, height = 0;
    int visibleHeight = 0, visibleTop = 0;
    Color barColor, frameColor, normalColor, draggedColor, hoveredColor, invalidColor;

    int thumbY = 0, thumbHeight = 0;
    bool hasOverflow = false;
    bool dragged = false;
    bool enabled = true;
    bool hovered = false;
    bool hoveredThumb = false;

    VerticalScrollBar();
    // 没有 set_c 不会 Discard 之前的对象。
    void Render(App& app);

    virtual void Update(App& app);
    virtual void Discard(App& app);
    virtual void PreUpdate(App& app);
};
