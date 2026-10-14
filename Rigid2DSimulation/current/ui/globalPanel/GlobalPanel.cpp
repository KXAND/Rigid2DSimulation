#include "GlobalPanel.h"

#include <memory>
#include <vector>

#include "SimulationApp.h"
#include "UserInterface.h"

GlobalPanel::GlobalPanel(App& app) : LocalizedFloatPanel(app, 400, 600, L"globalPanelTitle")
{
    vector<Control*> controls;
    savePathTextBox = std::make_shared<TextBox>(270);
    saveButton = std::make_shared<LocalizedButton>(80, L"saveButton");
    saveButton->onClick = [](App& app)
    {
        auto& simulationApp = static_cast<SimulationApp&>(app);
        simulationApp.Save(simulationApp.userInterface->globalPanel->savePathTextBox->value);
    };
    controls = {&*savePathTextBox, &*saveButton};
    saveControls = std::make_shared<HorizontalControlList>(controls);
    controls = {};
    loadPathTextBox = std::make_shared<TextBox>(270);
    loadButton = std::make_shared<LocalizedButton>(80, L"loadButton");
    loadButton->onClick = [](App& app)
    {
        auto& simulationApp = static_cast<SimulationApp&>(app);
        simulationApp.Load(simulationApp.userInterface->globalPanel->loadPathTextBox->value);
    };
    controls = {&*loadPathTextBox, &*loadButton};
    loadControls = std::make_shared<HorizontalControlList>(controls);
    controls = {};
    separator = std::make_shared<HorizontalSeparator>(400);
    detailsLabel = std::make_shared<GlobalDetailsLabel>();
    controls = {&*saveControls, &*loadControls, &*separator, &*detailsLabel};
    this->controls = std::make_shared<VerticalControlList>(controls);
    contentControl = &*this->controls;
    Init(app);
}
