#pragma once

#include <cstddef>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "Control.h"

class Button;
class FloatPanel;
class HorizontalStack;
class HorizontalDragBar;
class HorizontalSeparator;
class Label;
class LocalizedButton;
class LocalizedCheckBox;
class LocalizedRichLabel;
class LocalizedTextBox;
class RichLabel;
class RichTextBox;
class ShowFloatPanelButton;
class TextBox;
class VerticalStack;

class ControlContainer : public Control
{
public:
    // 容器独占子控件；借用引用有效至 Clear 或容器销毁。
    Control& Add(std::unique_ptr<Control> control);
    HorizontalStack& AddRow();
    VerticalStack& AddColumn();
    Button& AddButton(int width, std::wstring const& text, std::function<void(App&)> onClick);
    LocalizedButton& AddLocalizedButton(int width, std::wstring const& textId, std::function<void(App&)> onClick);
    ShowFloatPanelButton& AddPanelButton(FloatPanel& panel);
    TextBox& AddTextBox(int width);
    LocalizedTextBox& AddLocalizedTextBox(int width, std::wstring const& textId);
    RichTextBox& AddRichTextBox(int width, int height);
    Label& AddLabel(int width);
    RichLabel& AddRichLabel(int width, std::function<std::wstring(App&)> textProvider);
    LocalizedRichLabel& AddLocalizedRichLabel(int width, std::wstring const& textId);
    LocalizedCheckBox& AddLocalizedCheckBox(std::wstring const& textId);
    HorizontalDragBar& AddDragBar(int width);
    HorizontalSeparator& AddSeparator(int width);

    std::size_t GetSize() const;
    std::vector<Control*> const& GetControls() const;
    // 只显示前 size 个子项；隐藏时 Discard，但保留实例供动态列表复用。
    void SetVisibleCount(std::size_t size, App& app);
    void Clear(App& app);
    void Discard(App& app) override;
    void PreUpdate(App& app) override;

private:
    std::vector<std::unique_ptr<Control>> ownedControls;
    std::vector<Control*> controls;
};

class HorizontalStack : public ControlContainer
{
public:
    int GetWidth() const override;
    int GetHeight() const override;
    void Update(App& app) override;
};

class VerticalStack : public ControlContainer
{
public:
    int GetWidth() const override;
    int GetHeight() const override;
    void Update(App& app) override;
};
