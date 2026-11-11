#include "ControlList.h"

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

Control& ControlList::Add(std::unique_ptr<Control> control)
{
    if (!control)
    {
        throw std::invalid_argument("ControlList::Add requires a control");
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

HorizontalControlList& ControlList::AddRow()
{
    auto control = std::make_unique<HorizontalControlList>();
    auto& result = *control;
    Add(std::move(control));
    return result;
}

VerticalControlList& ControlList::AddColumn()
{
    auto control = std::make_unique<VerticalControlList>();
    auto& result = *control;
    Add(std::move(control));
    return result;
}

Button& ControlList::AddButton(int width, std::wstring const& text, std::function<void(App&)> onClick)
{
    auto control = std::make_unique<Button>(width);
    control->text = text;
    control->onClick = std::move(onClick);
    auto& result = *control;
    Add(std::move(control));
    return result;
}

LocalizedButton& ControlList::AddLocalizedButton(int width, std::wstring const& textId, std::function<void(App&)> onClick)
{
    auto control = std::make_unique<LocalizedButton>(width, textId);
    control->onClick = std::move(onClick);
    auto& result = *control;
    Add(std::move(control));
    return result;
}

ShowFloatPanelButton& ControlList::AddPanelButton(FloatPanel& panel)
{
    auto control = std::make_unique<ShowFloatPanelButton>(&panel);
    auto& result = *control;
    Add(std::move(control));
    return result;
}

TextBox& ControlList::AddTextBox(int width)
{
    auto control = std::make_unique<TextBox>(width);
    auto& result = *control;
    Add(std::move(control));
    return result;
}

LocalizedTextBox& ControlList::AddLocalizedTextBox(int width, std::wstring const& textId)
{
    auto control = std::make_unique<LocalizedTextBox>(textId, width);
    auto& result = *control;
    Add(std::move(control));
    return result;
}

RichTextBox& ControlList::AddRichTextBox(int width, int height)
{
    auto control = std::make_unique<RichTextBox>(width, height);
    auto& result = *control;
    Add(std::move(control));
    return result;
}

Label& ControlList::AddLabel(int width)
{
    auto control = std::make_unique<Label>();
    control->width = width;
    control->fixedWidth = true;
    auto& result = *control;
    Add(std::move(control));
    return result;
}

RichLabel& ControlList::AddRichLabel(int width, std::function<std::wstring(App&)> textProvider)
{
    auto control = std::make_unique<RichLabel>(width);
    control->textProvider = std::move(textProvider);
    auto& result = *control;
    Add(std::move(control));
    return result;
}

LocalizedRichLabel& ControlList::AddLocalizedRichLabel(int width, std::wstring const& textId)
{
    auto control = std::make_unique<LocalizedRichLabel>(width, textId);
    auto& result = *control;
    Add(std::move(control));
    return result;
}

LocalizedCheckBox& ControlList::AddLocalizedCheckBox(std::wstring const& textId)
{
    auto control = std::make_unique<LocalizedCheckBox>(textId);
    auto& result = *control;
    Add(std::move(control));
    return result;
}

HorizontalDragBar& ControlList::AddDragBar(int width)
{
    auto control = std::make_unique<HorizontalDragBar>(width);
    auto& result = *control;
    Add(std::move(control));
    return result;
}

HorizontalSeparator& ControlList::AddSeparator(int width)
{
    auto control = std::make_unique<HorizontalSeparator>(width);
    auto& result = *control;
    Add(std::move(control));
    return result;
}

std::size_t ControlList::GetSize() const
{
    return ownedControls.size();
}

std::vector<Control*> const& ControlList::GetControls() const
{
    return controls;
}

void ControlList::SetVisibleCount(std::size_t size, App& app)
{
    if (size > ownedControls.size())
    {
        throw std::out_of_range("ControlList::SetVisibleCount exceeds child count");
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

void ControlList::Clear(App& app)
{
    Discard(app);
    controls.clear();
    ownedControls.clear();
}

void ControlList::Discard(App& app)
{
    for (auto control : controls)
    {
        control->Discard(app);
    }
}

void ControlList::PreUpdate(App& app)
{
    for (auto control : controls)
    {
        control->PreUpdate(app);
    }
}

int HorizontalControlList::GetWidth() const
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

int HorizontalControlList::GetHeight() const
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

void HorizontalControlList::Update(App& app)
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

int VerticalControlList::GetWidth() const
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

int VerticalControlList::GetHeight() const
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

void VerticalControlList::Update(App& app)
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
