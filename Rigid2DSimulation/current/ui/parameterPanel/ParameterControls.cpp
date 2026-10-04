#include "ParameterControls.h"

#include <memory>
#include <vector>

#include "SceneParameter.h"
#include "SimulationApp.h"

ParameterNameTextBox::ParameterNameTextBox(int id) : id(id), TextBox(270) {};

void ParameterNameTextBox::Sync(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    value = simulationApp.parameters[id]->name;
}

void ParameterNameTextBox::OnDone(App& app) const
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    simulationApp.parameters[id]->name = value;
}

ParameterValueDragBar::ParameterValueDragBar(int id) : id(id), HorizontalDragBar(270) {}

void ParameterValueDragBar::Sync(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    SetNormalizedNodePosition(simulationApp.parameters[id]->value);
}

void ParameterValueDragBar::Upload(App& app) const
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    simulationApp.parameters[id]->value = GetNormalizedValue();
}

ParameterValueTextBox::ParameterValueTextBox(int id) : id(id), TextBox(80) {};

void ParameterValueTextBox::Sync(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    value = FormatFixed2(simulationApp.parameters[id]->value);
}

void ParameterValueTextBox::OnDone(App& app) const
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    simulationApp.parameters[id]->value = ParseDouble(value);
}

ParameterControlRow::ParameterControlRow(int id)
{
    vector<Control*> controls;
    nameTextBox = std::make_shared<ParameterNameTextBox>(id);
    deleteButton = std::make_shared<ParameterDeletionButton>(id);
    controls = {&*nameTextBox, &*deleteButton};
    nameControls = std::make_shared<HorizontalControlList>(controls);
    controls = {};
    valueDragBar = std::make_shared<ParameterValueDragBar>(id);
    valueTextBox = std::make_shared<ParameterValueTextBox>(id);
    controls = {&*valueDragBar, &*valueTextBox};
    valueControls = std::make_shared<HorizontalControlList>(controls);
    controls = {};
    separator = std::make_shared<HorizontalSeparator>(400);
    controls = {&*nameControls, &*valueControls, &*separator};
}

void ParameterControlList::Update(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    controls.clear();
    for (int i = (parameterRows.size()), iEnd = int(simulationApp.parameters.size()); i < iEnd; ++i)
    {
        parameterRows.push_back(std::make_shared<ParameterControlRow>(i));
    }
    for (int i = (0), iEnd = int(simulationApp.parameters.size()); i < iEnd; ++i)
    {
        controls.push_back(&*parameterRows[i]);
    }
    for (int i = (simulationApp.parameters.size()), iEnd = int(parameterRows.size()); i < iEnd; ++i)
    {
        parameterRows[i]->Discard(app);
    }
    VerticalControlList::Update(app);
}
