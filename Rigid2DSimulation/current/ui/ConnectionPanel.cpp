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
	rtb = std::make_shared<ConnectionScriptEditor>();
	bt_ok = std::make_shared<ApplyConnectionButton>(); bt_del = std::make_shared<DeleteConnectionButton>();
	tmp = { &*bt_ok, &*bt_del };
	clx_bt = std::make_shared<HorizontalControlList>(tmp);
	tmp = {};
	spy = std::make_shared<HorizontalSeparator>(400);
}
void ConnectionEditorControls::Update(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	auto* selectedConnection = simulationApp.connectionSelecting;
	if (con != selectedConnection) {
		con = selectedConnection;
		for (auto c : cs) { c->Discard(app); }
		if (con) { cs = { &*rtb, &*clx_bt, &*spy }; }
		else { cs = {}; }
	}
	VerticalControlList::Update(app);
}
void ConnectionEditorControls::Discard(App& app) {
	VerticalControlList::Discard(app);
	con = NULL; cs = {};
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
	cly_con = std::make_shared<ConnectionEditorControls>(); rlb_con = std::make_shared<ConnectionDetailsLabel>();
	tmp = { &*cly_con, &*rlb_con };
	cly = std::make_shared<VerticalControlList>(tmp);
	c = &*cly;
	Init(app);
}



