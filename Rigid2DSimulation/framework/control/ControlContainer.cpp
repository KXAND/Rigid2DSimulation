#include "ControlContainer.h"

#include <stdexcept>
#include <utility>

#include "Button.h"
#include "CheckBox.h"
#include "DragBar.h"
#include "Label.h"
#include "RichLabel.h"
#include "RichTextBox.h"
#include "Separator.h"
#include "TextBox.h"

Control& ControlContainer::Add(std::unique_ptr<Control> control)
{
    if (!control)
    {
        throw std::invalid_argument("ControlContainer::Add requires a control");
    }
    bool showNewControl = controls.size() == ownedControls.size();
    // 先预留视图容量，避免接管成功后因分配失败留下未登记的子项。
    if (showNewControl && controls.size() == controls.capacity())
    {
        controls.reserve(controls.empty() ? 1 : controls.size() * 2);
    }
    auto& child = *control;
    ownedControls.push_back(std::move(control));
    if (showNewControl)
    {
        controls.push_back(&child);
    }
    return child;
}

HorizontalStack& ControlContainer::AddRow()
{
    auto control = std::make_unique<HorizontalStack>();
    auto& result = *control;
    Add(std::move(control));
    return result;
}

VerticalStack& ControlContainer::AddColumn()
{
    auto control = std::make_unique<VerticalStack>();
    auto& result = *control;
    Add(std::move(control));
    return result;
}

Button& ControlContainer::AddButton(int width, std::wstring const& text, std::function<void(App&)> onClick)
{
    auto control = std::make_unique<Button>(width);
    control->text = text;
    control->onClick = std::move(onClick);
    auto& result = *control;
    Add(std::move(control));
    return result;
}

LocalizedButton& ControlContainer::AddLocalizedButton(int width, std::wstring const& textId, std::function<void(App&)> onClick)
{
    auto control = std::make_unique<LocalizedButton>(width, textId);
    control->onClick = std::move(onClick);
    auto& result = *control;
    Add(std::move(control));
    return result;
}

ShowFloatPanelButton& ControlContainer::AddPanelButton(FloatPanel& panel)
{
    auto control = std::make_unique<ShowFloatPanelButton>(&panel);
    auto& result = *control;
    Add(std::move(control));
    return result;
}

TextBox& ControlContainer::AddTextBox(int width)
{
    auto control = std::make_unique<TextBox>(width);
    auto& result = *control;
    Add(std::move(control));
    return result;
}

LocalizedTextBox& ControlContainer::AddLocalizedTextBox(int width, std::wstring const& textId)
{
    auto control = std::make_unique<LocalizedTextBox>(textId, width);
    auto& result = *control;
    Add(std::move(control));
    return result;
}

RichTextBox& ControlContainer::AddRichTextBox(int width, int height)
{
    auto control = std::make_unique<RichTextBox>(width, height);
    auto& result = *control;
    Add(std::move(control));
    return result;
}

Label& ControlContainer::AddLabel(int width)
{
    auto control = std::make_unique<Label>();
    control->width = width;
    control->fixedWidth = true;
    auto& result = *control;
    Add(std::move(control));
    return result;
}

RichLabel& ControlContainer::AddRichLabel(int width, std::function<std::wstring(App&)> textProvider)
{
    auto control = std::make_unique<RichLabel>(width);
    control->textProvider = std::move(textProvider);
    auto& result = *control;
    Add(std::move(control));
    return result;
}

LocalizedRichLabel& ControlContainer::AddLocalizedRichLabel(int width, std::wstring const& textId)
{
    auto control = std::make_unique<LocalizedRichLabel>(width, textId);
    auto& result = *control;
    Add(std::move(control));
    return result;
}

LocalizedCheckBox& ControlContainer::AddLocalizedCheckBox(std::wstring const& textId)
{
    auto control = std::make_unique<LocalizedCheckBox>(textId);
    auto& result = *control;
    Add(std::move(control));
    return result;
}

HorizontalDragBar& ControlContainer::AddDragBar(int width)
{
    auto control = std::make_unique<HorizontalDragBar>(width);
    auto& result = *control;
    Add(std::move(control));
    return result;
}

HorizontalSeparator& ControlContainer::AddSeparator(int width)
{
    auto control = std::make_unique<HorizontalSeparator>(width);
    auto& result = *control;
    Add(std::move(control));
    return result;
}

std::size_t ControlContainer::GetSize() const
{
    return ownedControls.size();
}

std::vector<Control*> const& ControlContainer::GetControls() const
{
    return controls;
}

void ControlContainer::SetVisibleCount(std::size_t size, App& app)
{
    if (size > ownedControls.size())
    {
        throw std::out_of_range("ControlContainer::SetVisibleCount exceeds child count");
    }
    controls.reserve(size);
    for (std::size_t i = size; i < controls.size(); ++i)
    {
        controls[i]->Discard(app);
    }
    controls.clear();
    for (std::size_t i = 0; i < size; ++i)
    {
        controls.push_back(ownedControls[i].get());
    }
}

void ControlContainer::Clear(App& app)
{
    Discard(app);
    controls.clear();
    ownedControls.clear();
}

void ControlContainer::Discard(App& app)
{
    for (auto control : controls)
    {
        control->Discard(app);
    }
}

void ControlContainer::Render(App& app)
{
    for (auto control : controls)
    {
        control->Render(app);
    }
}

void ControlContainer::PreUpdate(App& app)
{
    for (auto control : controls)
    {
        // 父容器的裁剪范围可能已变化，命中测试不能沿用上一帧的范围。
        control->viewport = viewport;
        control->PreUpdate(app);
    }
}

int HorizontalStack::GetWidth() const
{
    if (GetControls().empty())
    {
        return 0;
    }
    int totalWidth = 0;
    for (auto control : GetControls())
    {
        totalWidth += control->GetWidth();
    }
    return totalWidth;
}

int HorizontalStack::GetHeight() const
{
    int maxHeight = 0;
    for (auto control : GetControls())
    {
        if (control->GetHeight() > maxHeight)
        {
            maxHeight = control->GetHeight();
        }
    }
    return maxHeight;
}

void HorizontalStack::Update(App& app)
{
    int currentX = topLeft.x;
    for (auto control : GetControls())
    {
        control->viewport = viewport;
        control->topLeft.x = currentX;
        control->topLeft.y = topLeft.y;
        // 成员是要先 Update 然后再求宽度的。
        control->Update(app);
        currentX += control->GetWidth();
    }
}

int VerticalStack::GetWidth() const
{
    int maxWidth = 0;
    for (auto control : GetControls())
    {
        if (control->GetWidth() > maxWidth)
        {
            maxWidth = control->GetWidth();
        }
    }
    return maxWidth;
}

int VerticalStack::GetHeight() const
{
    if (GetControls().empty())
    {
        return 0;
    }
    int totalHeight = 0;
    for (auto control : GetControls())
    {
        totalHeight += control->GetHeight();
    }
    return totalHeight;
}

void VerticalStack::Update(App& app)
{
    int currentY = topLeft.y;
    for (auto control : GetControls())
    {
        control->viewport = viewport;
        control->topLeft.x = topLeft.x;
        control->topLeft.y = currentY;
        control->Update(app);
        currentY += control->GetHeight();
    }
}
