#include "CommandPanel.h"

#include <memory>
#include <vector>

#include "SimulationApp.h"

CommandPanel::CommandPanel(App& app) : LocalizedFloatPanel(app, 600, 600, L"commandPanelTitle")
{
    scriptEditor = std::make_shared<RichTextBox>(560, 500);
    scriptEditor->valueProvider = [](App& app)
    {
        return static_cast<SimulationApp&>(app).scriptRuntime.temporaryScriptText;
    };
    scriptEditor->onEditFinished = [](App& app, std::wstring const& text)
    {
        static_cast<SimulationApp&>(app).scriptRuntime.temporaryScriptText = text;
    };

    compileButton = std::make_shared<LocalizedButton>(200, L"compile");
    compileButton->onClick = [](App& app)
    {
        auto& simulationApp = static_cast<SimulationApp&>(app);
        simulationApp.scriptRuntime.ApplySceneScript();
    };

    layout = std::make_shared<VerticalControlList>(std::vector<Control*>{scriptEditor.get(), compileButton.get()});
    contentControl = layout.get();
    Init(app);
}
