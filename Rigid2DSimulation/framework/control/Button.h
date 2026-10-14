#pragma once
#include <functional>
#include <string>

#include "Font.h"
#include "Control.h"

class Button : public Control
{
public:
    DVector2 gap;
    double depth = 0;
    wstring text;
    int width = 0, height = 0;
    Color textColor, frameColor, normalColor, chosenColor, hoveredColor, invalidColor;

    bool chosen = false;
    bool enabled = true;
    bool hovered = false;

    Button(int width = 0);
    int GetWidth() const override;
    int GetHeight() const override;

    void Render(App& app);
    // 在原点击判定位置同步执行；为空时不处理，不保存传入的 App。
    std::function<void(App&)> onClick;
    // 在点击处理之前读取标题，保留动态按钮原来的显示时序。
    std::function<std::wstring(App&)> textProvider;

    void Update(App& app) override;
    void Discard(App& app) override;
    void PreUpdate(App& app) override;
};
class LocalizedButton : public Button
{
public:
    wstring textId;
    LocalizedButton() = default;
    LocalizedButton(int width, wstring const& id);
    void Update(App& app) override;
};

class FloatPanel;
class ShowFloatPanelButton : public Button
{
public:
    FloatPanel* floatPanel = NULL;
    ShowFloatPanelButton() = default;
    ShowFloatPanelButton(FloatPanel* floatPanel);
    // 点击回调绑定自身，控件保持稳定地址，由外部通过指针持有。
    ShowFloatPanelButton(ShowFloatPanelButton const&) = delete;
    ShowFloatPanelButton& operator=(ShowFloatPanelButton const&) = delete;
    void Update(App& app) override;
};
