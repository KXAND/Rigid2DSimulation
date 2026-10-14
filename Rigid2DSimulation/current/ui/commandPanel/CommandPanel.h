#pragma once

#include <memory>

#include "Button.h"
#include "ControlList.h"
#include "FloatPanel.h"
#include "RichTextBox.h"

class CommandPanel : public LocalizedFloatPanel
{
public:
    std::shared_ptr<RichTextBox> scriptEditor;
    std::shared_ptr<LocalizedButton> compileButton;
    std::shared_ptr<VerticalControlList> layout;
    CommandPanel(App& app);
};
