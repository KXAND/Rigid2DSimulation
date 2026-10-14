#include "ParameterPanel.h"

#include <memory>
#include <vector>

#include "SceneParameter.h"
#include "SimulationApp.h"

ParameterPanel::ParameterPanel(App& app) : LocalizedFloatPanel(app, 400, 600, L"parameterPanelTitle")
{
    vector<Control*> controls;
    parameterList = std::make_shared<ParameterControlList>();
    addButton = std::make_shared<LocalizedButton>(160, L"addParameterButton");
    addButton->onClick = [](App& app)
    {
        auto& simulationApp = static_cast<SimulationApp&>(app);
        simulationApp.scriptRuntime.parameters.push_back(
            std::make_shared<SceneParameter>(simulationApp.GetLocalizedString(L"unnamedParameter")));
    };
    controls = {&*parameterList, &*addButton};
    this->controls = std::make_shared<VerticalControlList>(controls);
    contentControl = &*this->controls;
    Init(app);
}
