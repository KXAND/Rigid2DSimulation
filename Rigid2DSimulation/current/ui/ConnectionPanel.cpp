#include "ConnectionPanel.h"
#include "SimulationApp.h"
#include "Connection.h"

ConnectionScriptEditor::ConnectionScriptEditor() : RichTextBox(360, 400) {}
void ConnectionScriptEditor::Sync(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); value = simulationApp.connectionSelecting->temporaryScriptText; }
void ConnectionScriptEditor::Upload(App& app) const { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.connectionSelecting->temporaryScriptText = value; }

ApplyConnectionButton::ApplyConnectionButton() : LocalizedButton(70, L"applyConnectionButton") {}
void ApplyConnectionButton::OnClick(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	auto& selectedConnection = *simulationApp.connectionSelecting;
	selectedConnection.scriptText = selectedConnection.temporaryScriptText;
	selectedConnection.RefreshFromScript(simulationApp);
}
DeleteConnectionButton::DeleteConnectionButton() : LocalizedButton(70, L"deleteConnectionButton") {}
void DeleteConnectionButton::OnClick(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.connectionSelecting->isMarkedForDeletion = true; simulationApp.isSceneChanged = true; }

ConnectionEditorControls::ConnectionEditorControls() {
	vector<Control*> controls;
	scriptEditor = std::make_shared<ConnectionScriptEditor>();
	applyButton = std::make_shared<ApplyConnectionButton>(); deleteButton = std::make_shared<DeleteConnectionButton>();
	controls = { &*applyButton, &*deleteButton };
	actionButtons = std::make_shared<HorizontalControlList>(controls);
	controls = {};
	separator = std::make_shared<HorizontalSeparator>(400);
}
void ConnectionEditorControls::Update(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	auto* appSelectedConnection = simulationApp.connectionSelecting;
	if (selectedConnection != appSelectedConnection) {
		selectedConnection = appSelectedConnection;
		for (auto control : controls) { control->Discard(app); }
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
	if (!selectedConnection) { text = simulationApp.GetLocalizedString(L"noConnectionSelected"); return; }
	auto& connection = *selectedConnection;
	text.clear();

	wstring type;
	switch (connection.type) {
	case kRopeConnection: type = simulationApp.GetLocalizedString(L"ropeConnection"); break;
	case kLinkConnection: type = simulationApp.GetLocalizedString(L"linkConnection"); break;
	case kSpringConnection: type = simulationApp.GetLocalizedString(L"springConnection"); break;
	case kCordConnection: type = simulationApp.GetLocalizedString(L"cordConnection"); break;
	}
	text += simulationApp.GetLocalizedString(L"connectionType") + type + L"\n";
	text += simulationApp.GetLocalizedString(L"connectionLength") + FormatFixed2(connection.length) + L"\n";
	text += simulationApp.GetLocalizedString(L"springConstant") + FormatFixed2(connection.springConstant) + L"\n";
}

ConnectionPanel::ConnectionPanel(App& app) : LocalizedFloatPanel(app, 400, 600, L"connectionPanelTitle") {
	vector<Control*> controls;
	editorControls = std::make_shared<ConnectionEditorControls>(); detailsLabel = std::make_shared<ConnectionDetailsLabel>();
	controls = { &*editorControls, &*detailsLabel };
	this->controls = std::make_shared<VerticalControlList>(controls);
	contentControl = &*this->controls;
	Init(app);
}



