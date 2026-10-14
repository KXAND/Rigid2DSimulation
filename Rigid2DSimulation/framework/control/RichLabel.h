#pragma once
#include <functional>
#include <string>

#include "Control.h"
#include "Font.h"

class RichLabel : public Control
{
public:
    DVector2 gap;
    double depth = 0;
    int width = 0, height = 0;
    bool fixedWidth = false;
    bool fixedHeight = false;

    Color textColor;
    wstring text;
    // 默认 Sync 在测量、绘制之前调用；派生类重写 Sync 时替代此行为。
    std::function<std::wstring(App&)> textProvider;

    RichLabel(int width = 0);
    int GetWidth() const override;
    int GetHeight() const override;

    int GetLineWidth() const;
    void Render(App& app);
    virtual void Sync(App& app);
    void Update(App& app);
};
class LocalizedRichLabel : public RichLabel
{
public:
    wstring textId;
    LocalizedRichLabel() = default;
    LocalizedRichLabel(int width, wstring const& id);
    void Sync(App& app) override;
};
