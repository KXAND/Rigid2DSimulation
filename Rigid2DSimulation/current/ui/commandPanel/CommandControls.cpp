#include "CommandControls.h"

#include "SimulationApp.h"

CommandScriptEditor::CommandScriptEditor() : RichTextBox(560, 500) {}

void CommandScriptEditor::Sync(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    value = simulationApp.temporaryScriptText;
}

void CommandScriptEditor::OnDone(App& app) const
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    simulationApp.temporaryScriptText = value;
}
