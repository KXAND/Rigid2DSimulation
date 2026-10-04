#pragma once

#include <memory>

#include "CommandButtons.h"
#include "CommandControls.h"
#include "ControlList.h"
#include "FloatPanel.h"

struct CommandPanel : LocalizedFloatPanel
{
    std::shared_ptr<CommandScriptEditor> scriptEditor;
    std::shared_ptr<ScriptExecutionButton> compileButton;
    std::shared_ptr<VerticalControlList> controls;
    CommandPanel(App& app);
};
