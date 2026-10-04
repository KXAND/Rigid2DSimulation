#include "ParameterButtons.h"

#include <memory>

#include "SceneParameter.h"
#include "SimulationApp.h"

ParameterDeletionButton::ParameterDeletionButton(int id) : id(id), LocalizedButton(80, L"deleteParameterButton") {}

void ParameterDeletionButton::OnClick(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    simulationApp.parameters[id]->isMarkedForDeletion = true;
}

ParameterAdditionButton::ParameterAdditionButton() : LocalizedButton(160, L"addParameterButton") {}

void ParameterAdditionButton::OnClick(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    simulationApp.parameters.push_back(std::make_shared<SceneParameter>(simulationApp.GetLocalizedString(L"unnamedParameter")));
}
