#include "CommandButtons.h"

#include "SimulationApp.h"
#include "Value.h"

ScriptExecutionButton::ScriptExecutionButton() : LocalizedButton(200, L"compile") {}

void ScriptExecutionButton::OnClick(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    simulationApp.scriptText = simulationApp.temporaryScriptText;
    Execute(simulationApp.currentScope, Compile(simulationApp.scriptText));
}
