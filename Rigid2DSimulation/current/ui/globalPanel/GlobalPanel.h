#pragma once

#include <memory>
#include <string>

#include "Button.h"
#include "ControlList.h"
#include "FloatPanel.h"
#include "RichLabel.h"
#include "Separator.h"
#include "TextBox.h"

class GlobalPanel : public LocalizedFloatPanel
{
public:
    std::shared_ptr<TextBox> savePathTextBox;
    std::shared_ptr<LocalizedButton> saveButton;
    std::shared_ptr<HorizontalControlList> saveRow;
    std::shared_ptr<TextBox> loadPathTextBox;
    std::shared_ptr<LocalizedButton> loadButton;
    std::shared_ptr<HorizontalControlList> loadRow;
    std::shared_ptr<HorizontalSeparator> separator;
    std::shared_ptr<RichLabel> detailsLabel;
    std::shared_ptr<VerticalControlList> layout;

    GlobalPanel(App& app);

private:
    void BuildSaveRow();
    void BuildLoadRow();
    static std::wstring GetDetailsText(App& app);
};
