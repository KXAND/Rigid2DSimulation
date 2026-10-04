#include "CommandPanel.h"

#include <memory>
#include <vector>

CommandPanel::CommandPanel(App& app) : LocalizedFloatPanel(app, 600, 600, L"commandPanelTitle")
{
    vector<Control*> controls;
    scriptEditor = std::make_shared<CommandScriptEditor>();
    compileButton = std::make_shared<ScriptExecutionButton>();
    controls = {&*scriptEditor, &*compileButton};
    this->controls = std::make_shared<VerticalControlList>(controls);
    contentControl = &*this->controls;
    Init(app);
}
