#pragma once

#include <memory>

#include "Button.h"
#include "CommandControls.h"
#include "ControlList.h"
#include "FloatPanel.h"

class CommandPanel : public LocalizedFloatPanel
{
public:
    std::shared_ptr<CommandScriptEditor> scriptEditor;
    std::shared_ptr<LocalizedButton> compileButton;
    std::shared_ptr<VerticalControlList> controls;
    CommandPanel(App& app);
};
