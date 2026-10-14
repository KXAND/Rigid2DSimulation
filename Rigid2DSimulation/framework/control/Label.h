#pragma once
#include <functional>
#include <string>

#include "Font.h"
#include "Control.h"

class Label : public Control
{
public:
    DVector2 gap;
    double depth = 0;
    int width = 0, height = 0;
    bool fixedWidth = false;

    Color textColor;
    wstring text;
    // 默认 Sync 在测量、绘制之前读取；派生类覆盖 Sync 时替代此行为。
    std::function<std::wstring(App&)> textProvider;

    Label(bool big = false);
    int GetWidth() const override;
    int GetHeight() const override;

    void Render(App& app);
    virtual void Sync(App& app);
    void Update(App& app) override;
};
class LocalizedLabel : public Label
{
public:
    wstring textId;
    LocalizedLabel() = default;
    LocalizedLabel(wstring const& id, bool big = false);
    void Sync(App& app) override;
};
