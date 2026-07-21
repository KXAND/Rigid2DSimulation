#include "ConnectionPanel.h"
#include "SimulationApp.h"
#include "Connection.h"

ConnectionScriptEditor::ConnectionScriptEditor() : RichTextBox(360, 400) {}
void ConnectionScriptEditor::Sync(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); value = simulationApp.connectionSelecting->temporaryScriptText; }
void ConnectionScriptEditor::Upload(App& app) const { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.connectionSelecting->temporaryScriptText = value; }

ApplyConnectionButton::ApplyConnectionButton() : LocalizedButton(70, L"bt_con_ok") {}
void ApplyConnectionButton::OnClick(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	auto& selectedConnection = *simulationApp.connectionSelecting;
	selectedConnection.scriptText = selectedConnection.temporaryScriptText;
	selectedConnection.RefreshFromScript(simulationApp);
}
DeleteConnectionButton::DeleteConnectionButton() : LocalizedButton(70, L"bt_con_del") {}
void DeleteConnectionButton::OnClick(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.connectionSelecting->isMarkedForDeletion = true; simulationApp.isSceneChanged = true; }

ConnectionEditorControls::ConnectionEditorControls() {
	vector<Control*> tmp;
	scriptEditor = std::make_shared<ConnectionScriptEditor>();
	applyButton = std::make_shared<ApplyConnectionButton>(); deleteButton = std::make_shared<DeleteConnectionButton>();
	tmp = { &*applyButton, &*deleteButton };
	actionButtons = std::make_shared<HorizontalControlList>(tmp);
	tmp = {};
	separator = std::make_shared<HorizontalSeparator>(400);
}
void ConnectionEditorControls::Update(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	auto* selectedConnection = simulationApp.connectionSelecting;
	if (selectedConnection != selectedConnection) {
		selectedConnection = selectedConnection;
		for (auto c : controls) { c->Discard(app); }
		if (selectedConnection) { controls = { &*scriptEditor, &*actionButtons, &*separator }; }
		else { controls = {}; }
	}
	VerticalControlList::Update(app);
}
void ConnectionEditorControls::Discard(App& app) {
	VerticalControlList::Discard(app);
	selectedConnection = NULL; controls = {};
}

void ConnectionDetailsLabel::Sync(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	auto* selectedConnection = simulationApp.connectionSelecting;
	if (!selectedConnection) { text = simulationApp.GetLocalizedString(L"conn_not_sel"); return; }
	auto& connection = *selectedConnection;
	text.clear();

	wstring type;
	switch (connection.type) {
	case CON_ROPE: type = simulationApp.GetLocalizedString(L"conn_rope"); break;
	case CON_LINK: type = simulationApp.GetLocalizedString(L"conn_link"); break;
	case CON_SPRING: type = simulationApp.GetLocalizedString(L"conn_spring"); break;
	case CON_CORD: type = simulationApp.GetLocalizedString(L"conn_cord"); break;
	}
	text += simulationApp.GetLocalizedString(L"conn_type") + type + L"\n";
	text += simulationApp.GetLocalizedString(L"conn_len") + FormatFixed2(connection.len) + L"\n";
	text += simulationApp.GetLocalizedString(L"conn_hooke") + FormatFixed2(connection.hooke) + L"\n";
}

ConnectionPanel::ConnectionPanel(App& app) : LocalizedFloatPanel(app, 400, 600, L"fpn_con") {
	vector<Control*> tmp;
	editorControls = std::make_shared<ConnectionEditorControls>(); detailsLabel = std::make_shared<ConnectionDetailsLabel>();
	tmp = { &*editorControls, &*detailsLabel };
	controls = std::make_shared<VerticalControlList>(tmp);
	contentControl = &*controls;
	Init(app);
}



