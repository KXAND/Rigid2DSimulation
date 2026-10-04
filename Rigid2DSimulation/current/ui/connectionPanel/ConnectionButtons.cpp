#include "ConnectionButtons.h"

#include "Connection.h"
#include "SimulationApp.h"

ConnectionScriptApplicationButton::ConnectionScriptApplicationButton() : LocalizedButton(70, L"applyConnectionButton") {}

void ConnectionScriptApplicationButton::OnClick(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    auto& selectedConnection = *simulationApp.connectionSelecting;
    selectedConnection.scriptText = selectedConnection.temporaryScriptText;
    selectedConnection.RefreshFromScript(simulationApp);
}

ConnectionDeletionButton::ConnectionDeletionButton() : LocalizedButton(70, L"deleteConnectionButton") {}

void ConnectionDeletionButton::OnClick(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    simulationApp.connectionSelecting->isMarkedForDeletion = true;
    simulationApp.physicsWorld.isSceneChanged = true;
}
