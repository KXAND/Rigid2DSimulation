#include "ParameterPanel.h"
#include "SimulationApp.h"
#include "SceneParameter.h"

ParameterNameTextBox::ParameterNameTextBox(int id) : id(id), TextBox(270) {};
void ParameterNameTextBox::Sync(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); value = simulationApp.parameters[id]->name; }
void ParameterNameTextBox::OnDone(App& app) const { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.parameters[id]->name = value; }
DeleteParameterButton::DeleteParameterButton(int id) : id(id), LocalizedButton(80, L"bt_del_par") {}
void DeleteParameterButton::OnClick(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.parameters[id]->isMarkedForDeletion = true; }
ParameterValueDragBar::ParameterValueDragBar(int id) : id(id), HorizontalDragBar(270) {}
void ParameterValueDragBar::Sync(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); SetNormalizedNodePosition(simulationApp.parameters[id]->value); }
void ParameterValueDragBar::Upload(App& app) const { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.parameters[id]->value = GetNormalizedValue(); }
ParameterValueTextBox::ParameterValueTextBox(int id) : id(id), TextBox(80) {};
void ParameterValueTextBox::Sync(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); value = FormatFixed2(simulationApp.parameters[id]->value); }
void ParameterValueTextBox::OnDone(App& app) const { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.parameters[id]->value = wtof(value); }

ParameterControlRow::ParameterControlRow(int id) {
	vector<Control*> tmp;
	nameTextBox = std::make_shared<ParameterNameTextBox>(id); deleteButton = std::make_shared<DeleteParameterButton>(id);
	tmp = { &*nameTextBox, &*deleteButton };
	nameControls = std::make_shared<HorizontalControlList>(tmp);
	tmp = {};
	valueDragBar = std::make_shared<ParameterValueDragBar>(id); valueTextBox = std::make_shared<ParameterValueTextBox>(id);
	tmp = { &*valueDragBar, &*valueTextBox };
	valueControls = std::make_shared<HorizontalControlList>(tmp);
	tmp = {};
	separator = std::make_shared<HorizontalSeparator>(400);
	controls = { &*nameControls, &*valueControls, &*separator };
}
void ParameterControlList::Update(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	controls.clear();
	for (int i = (parameterRows.size()), iEnd = int(simulationApp.parameters.size()); i < iEnd; ++i) { parameterRows.push_back(std::make_shared<ParameterControlRow>(i)); }
	for (int i = (0), iEnd = int(simulationApp.parameters.size()); i < iEnd; ++i) { controls.push_back(&*parameterRows[i]); }
	for (int i = (simulationApp.parameters.size()), iEnd = int(parameterRows.size()); i < iEnd; ++i) { parameterRows[i]->Discard(app); }
	VerticalControlList::Update(app);
}

AddParameterButton::AddParameterButton() : LocalizedButton(160, L"addButton") {}
void AddParameterButton::OnClick(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.parameters.push_back(std::make_shared<SceneParameter>(simulationApp.GetLocalizedString(L"unnamed_par"))); }

ParameterPanel::ParameterPanel(App& app) : LocalizedFloatPanel(app, 400, 600, L"fpn_par") {
	vector<Control*> tmp;
	parameterList = std::make_shared<ParameterControlList>();
	addButton = std::make_shared<AddParameterButton>();
	tmp = { &*parameterList, &*addButton };
	controls = std::make_shared<VerticalControlList>(tmp);
	contentControl = &*controls;
	Init(app);
}



