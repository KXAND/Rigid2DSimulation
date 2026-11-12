#include "CommandPanel.h"

#include <memory>
#include <utility>

#include "RichTextBox.h"
#include "SimulationApp.h"

CommandPanel::CommandPanel(App& app) : LocalizedFloatPanel(app.screen.Rect(), 600, 600, L"commandPanelTitle")
{
    auto layout = std::make_unique<VerticalStack>();
    auto& scriptEditor = layout->AddRichTextBox(560, 500);
    scriptEditor.valueProvider = [](App& app)
    {
        return static_cast<SimulationApp&>(app).scriptRuntime.temporaryScriptText;
    };
    scriptEditor.onEditFinished = [](App& app, std::wstring const& text)
    {
        static_cast<SimulationApp&>(app).scriptRuntime.temporaryScriptText = text;
    };

    layout->AddLocalizedButton(200, L"compile", [](App& app)
    {
        auto& simulationApp = static_cast<SimulationApp&>(app);
        simulationApp.scriptRuntime.ApplySceneScript();
    });

    SetContent(std::move(layout));
}
