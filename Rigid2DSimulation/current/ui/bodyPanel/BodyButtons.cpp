#include "BodyButtons.h"

#include "Body.h"
#include "SimulationApp.h"

BodyScriptApplicationButton::BodyScriptApplicationButton() : LocalizedButton(70, L"applyBodyButton") {}

void BodyScriptApplicationButton::OnClick(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    auto& selectedBody = *simulationApp.editor.bodySelecting;
    selectedBody.scriptData.scriptText = selectedBody.scriptData.temporaryScriptText;
    simulationApp.scriptRuntime.RefreshBodyScript(selectedBody);
}

BodyDeletionButton::BodyDeletionButton() : LocalizedButton(70, L"deleteBodyButton") {}

void BodyDeletionButton::OnClick(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    simulationApp.editor.RequestBodyDeletion(simulationApp.physicsWorld);
}
