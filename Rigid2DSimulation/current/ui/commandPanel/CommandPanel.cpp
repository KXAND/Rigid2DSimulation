#include "CommandPanel.h"

#include <memory>
#include <vector>

#include "SimulationApp.h"

CommandPanel::CommandPanel(App& app) : LocalizedFloatPanel(app, 600, 600, L"commandPanelTitle")
{
    vector<Control*> controls;
    scriptEditor = std::make_shared<CommandScriptEditor>();
    compileButton = std::make_shared<LocalizedButton>(200, L"compile");
    compileButton->onClick = [](App& app)
    {
        auto& simulationApp = static_cast<SimulationApp&>(app);
        simulationApp.scriptRuntime.ApplySceneScript();
    };
    controls = {&*scriptEditor, &*compileButton};
    this->controls = std::make_shared<VerticalControlList>(controls);
    contentControl = &*this->controls;
    Init(app);
}
