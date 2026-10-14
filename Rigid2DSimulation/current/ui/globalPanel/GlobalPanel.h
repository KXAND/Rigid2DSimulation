#pragma once

#include <memory>

#include "ControlList.h"
#include "FloatPanel.h"
#include "Button.h"
#include "GlobalControls.h"
#include "Separator.h"
#include "TextBox.h"

class GlobalPanel : public LocalizedFloatPanel
{
public:
    std::shared_ptr<TextBox> savePathTextBox;
    std::shared_ptr<LocalizedButton> saveButton;
    std::shared_ptr<HorizontalControlList> saveControls;
    std::shared_ptr<TextBox> loadPathTextBox;
    std::shared_ptr<LocalizedButton> loadButton;
    std::shared_ptr<HorizontalControlList> loadControls;
    std::shared_ptr<HorizontalSeparator> separator;
    std::shared_ptr<GlobalDetailsLabel> detailsLabel;
    std::shared_ptr<VerticalControlList> controls;
    GlobalPanel(App& app);
};
