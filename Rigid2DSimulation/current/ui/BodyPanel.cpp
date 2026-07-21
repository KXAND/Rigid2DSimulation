#include "BodyPanel.h"
#include "SimulationApp.h"
#include "Body.h"
#include "Connection.h"

BodyScriptEditor::BodyScriptEditor() : RichTextBox(360, 400) {}
void BodyScriptEditor::Sync(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); value = simulationApp.bodySelecting->temporaryScriptText; }
void BodyScriptEditor::Upload(App& app) const { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.bodySelecting->temporaryScriptText = value; }

ApplyBodyButton::ApplyBodyButton() : LocalizedButton(70, L"bt_body_ok") {}
void ApplyBodyButton::OnClick(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	auto& selectedBody = *simulationApp.bodySelecting;
	selectedBody.scriptText = selectedBody.temporaryScriptText;
	selectedBody.RefreshFromScript(simulationApp);
}
DeleteBodyButton::DeleteBodyButton() : LocalizedButton(70, L"bt_body_del") {}
void DeleteBodyButton::OnClick(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.bodySelecting->isMarkedForDeletion = true; simulationApp.isSceneChanged = true; }


BodyConnectionLabel::BodyConnectionLabel(int id) : id(id), Label() {
	fixedWidth = true; width = 270;
}
void BodyConnectionLabel::Sync(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	auto* connection = simulationApp.bodySelecting->connections[id];
	wstring type;
	switch (connection->type) {
	case kRopeConnection: type = simulationApp.GetLocalizedString(L"conn_rope"); break;
	case kLinkConnection: type = simulationApp.GetLocalizedString(L"conn_link"); break;
	case kSpringConnection: type = simulationApp.GetLocalizedString(L"conn_spring"); break;
	case kCordConnection: type = simulationApp.GetLocalizedString(L"conn_cord"); break;
	}
	text = simulationApp.GetLocalizedString(L"conn") + std::to_wstring(id) + L": " + type;
}

SelectBodyConnectionCheckBox::SelectBodyConnectionCheckBox(int id) : id(id), LocalizedCheckBox(L"cb_body_con") {}
void SelectBodyConnectionCheckBox::Sync(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); checked = (simulationApp.connectionSelecting == simulationApp.bodySelecting->connections[id]); }
void SelectBodyConnectionCheckBox::Upload(App& app) const { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.connectionSelecting = checked ? simulationApp.bodySelecting->connections[id] : NULL; }

BodyConnectionControlRow::BodyConnectionControlRow(int id) {
	selectCheckBox = std::make_shared<SelectBodyConnectionCheckBox>(id); connectionLabel = std::make_shared<BodyConnectionLabel>(id);
	controls = { &*connectionLabel, &*selectCheckBox };
}
void BodyConnectionControlList::Update(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	auto& selectedBody = *simulationApp.bodySelecting;
	for (int i = (connectionRows.size()), iEnd = int(selectedBody.connections.size()); i < iEnd; ++i) {
		connectionRows.push_back(std::make_shared<BodyConnectionControlRow>(i));
	}
	controls.clear();
	for (int i = (0), iEnd = int(selectedBody.connections.size()); i < iEnd; ++i) {
		controls.push_back(&*connectionRows[i]);
	}
	for (int i = (selectedBody.connections.size()), iEnd = int(connectionRows.size()); i < iEnd; ++i) {
		connectionRows[i]->Discard(app);
	}
	VerticalControlList::Update(app);
}

BodyEditorControls::BodyEditorControls() {
	vector<Control*> tmp;
	scriptEditor = std::make_shared<BodyScriptEditor>();
	applyButton = std::make_shared<ApplyBodyButton>(); deleteButton = std::make_shared<DeleteBodyButton>();
	tmp = { &*applyButton, &*deleteButton };
	actionButtons = std::make_shared<HorizontalControlList>(tmp);
	tmp = {};
	connectionList = std::make_shared<BodyConnectionControlList>(); separator = std::make_shared<HorizontalSeparator>(400);
}
void BodyEditorControls::Update(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	auto* selectedBody = simulationApp.bodySelecting;
	if (selectedBody != selectedBody) {
		selectedBody = selectedBody;
		for (auto c : controls) { c->Discard(app); }
		if (selectedBody) { controls = { &*connectionList, &*scriptEditor, &*actionButtons, &*separator }; }
		else { controls = {}; }
	}
	VerticalControlList::Update(app);
}
void BodyEditorControls::Discard(App& app) {
	VerticalControlList::Discard(app);
	selectedBody = NULL; controls = {};
}

void BodyDetailsLabel::Sync(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	auto* selectedBody = simulationApp.bodySelecting;
	if (!selectedBody) { text = simulationApp.GetLocalizedString(L"body_not_sel"); return; }
	auto& body = *selectedBody;
	text.clear();

	text += simulationApp.GetLocalizedString(L"body_o") + ToWString(body.position) + L"\n";
	text += simulationApp.GetLocalizedString(L"body_v") + ToWString(body.velocity) + L"\n";
	text += simulationApp.GetLocalizedString(L"body_ang") + FormatFixed2(body.radian) + L"\n";
	text += simulationApp.GetLocalizedString(L"body_v_ang") + FormatFixed2(body.velocityAngular) + L"\n";
	text += simulationApp.GetLocalizedString(L"body_fixed") + std::to_wstring(body.fixed) + L"\n";
	text += simulationApp.GetLocalizedString(L"body_density") + FormatFixed2(body.density) + L"\n";
	text += simulationApp.GetLocalizedString(L"body_charge_density") + FormatFixed2(body.chargeDensity) + L"\n";
	text += simulationApp.GetLocalizedString(L"body_e") + FormatFixed2(body.elasticity) + L"\n";
	text += simulationApp.GetLocalizedString(L"body_mu_s") + FormatFixed2(body.frictionStatic) + L"\n";
	text += simulationApp.GetLocalizedString(L"body_mu_d") + FormatFixed2(body.frictionDynamic) + L"\n";
	text += simulationApp.GetLocalizedString(L"body_damp_v") + FormatFixed2(body.dampCoeff) + L"\n";
	text += simulationApp.GetLocalizedString(L"body_damp_v_ang") + FormatFixed2(body.dampCoeffAngular) + L"\n";
}

BodyPanel :: BodyPanel(App& app) : LocalizedFloatPanel(app, 400, 600, L"fpn_body") {
	vector<Control*> tmp;
	editorControls = std::make_shared<BodyEditorControls>(); detailsLabel = std::make_shared<BodyDetailsLabel>();
	tmp = { &*editorControls, &*detailsLabel };
	controls = std::make_shared<VerticalControlList>(tmp);
	contentControl = &*controls;
	Init(app);
}



