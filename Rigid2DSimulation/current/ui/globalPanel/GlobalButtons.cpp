#include "GlobalButtons.h"

#include "GlobalPanel.h"
#include "SimulationApp.h"
#include "UserInterface.h"

SceneSaveButton::SceneSaveButton() : LocalizedButton(80, L"saveButton") {}

void SceneSaveButton::OnClick(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    simulationApp.Save(simulationApp.userInterface->globalPanel->savePathTextBox->value);
}

SceneLoadButton::SceneLoadButton() : LocalizedButton(80, L"loadButton") {}

void SceneLoadButton::OnClick(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    simulationApp.Load(simulationApp.userInterface->globalPanel->loadPathTextBox->value);
}
