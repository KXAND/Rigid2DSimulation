#include "ConnectionPanel.h"
#include "SimulationApp.h"
#include "Connection.h"

ConnectionScriptEditor::ConnectionScriptEditor() : RichTextBox(360, 400) {}
void ConnectionScriptEditor::Sync(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); str = simulationApp.connectionSelecting->temporaryScriptText; }
void ConnectionScriptEditor::Upload(App& app) const { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.connectionSelecting->temporaryScriptText = str; }

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
		for (auto c : cs) { c->Discard(app); }
		if (selectedConnection) { cs = { &*scriptEditor, &*actionButtons, &*separator }; }
		else { cs = {}; }
	}
	VerticalControlList::Update(app);
}
void ConnectionEditorControls::Discard(App& app) {
	VerticalControlList::Discard(app);
	selectedConnection = NULL; cs = {};
}

void ConnectionDetailsLabel::Sync(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	auto* selectedConnection = simulationApp.connectionSelecting;
	if (!selectedConnection) { txt = simulationApp.getLocalizedString(L"conn_not_sel"); return; }
	auto& connection = *selectedConnection;
	txt.clear();

	wstring type;
	switch (connection.type) {
	case CON_ROPE: type = simulationApp.getLocalizedString(L"conn_rope"); break;
	case CON_LINK: type = simulationApp.getLocalizedString(L"conn_link"); break;
	case CON_SPRING: type = simulationApp.getLocalizedString(L"conn_spring"); break;
	case CON_CORD: type = simulationApp.getLocalizedString(L"conn_cord"); break;
	}
	txt += simulationApp.getLocalizedString(L"conn_type") + type + L"\n";
	txt += simulationApp.getLocalizedString(L"conn_len") + FormatFixed2(connection.len) + L"\n";
	txt += simulationApp.getLocalizedString(L"conn_hooke") + FormatFixed2(connection.hooke) + L"\n";
}

ConnectionPanel::ConnectionPanel(App& app) : LocalizedFloatPanel(app, 400, 600, L"fpn_con") {
	vector<Control*> tmp;
	editorControls = std::make_shared<ConnectionEditorControls>(); detailsLabel = std::make_shared<ConnectionDetailsLabel>();
	tmp = { &*editorControls, &*detailsLabel };
	controls = std::make_shared<VerticalControlList>(tmp);
	c = &*controls;
	Init(app);
}



