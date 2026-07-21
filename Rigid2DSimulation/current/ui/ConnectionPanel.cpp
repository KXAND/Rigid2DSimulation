#include "ConnectionPanel.h"
#include "SimulationApp.h"
#include "Connection.h"

RTbCon::RTbCon() : RichTextbox(360, 400) {}
void RTbCon::Sync(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); str = simulationApp.connectionSelecting->temporaryScriptText; }
void RTbCon::Upload(App& app) const { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.connectionSelecting->temporaryScriptText = str; }

BtConOK::BtConOK() : BtLan(70, L"bt_con_ok") {}
void BtConOK::OnClick(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	auto& selectedConnection = *simulationApp.connectionSelecting;
	selectedConnection.scriptText = selectedConnection.temporaryScriptText;
	selectedConnection.RefreshFromScript(simulationApp);
}
BtConDel::BtConDel() : BtLan(70, L"bt_con_del") {}
void BtConDel::OnClick(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.connectionSelecting->isMarkedForDeletion = true; simulationApp.isSceneChanged = true; }

ClyCon::ClyCon() {
	vector<Control*> tmp;
	rtb = std::make_shared<RTbCon>();
	bt_ok = std::make_shared<BtConOK>(); bt_del = std::make_shared<BtConDel>();
	tmp = { &*bt_ok, &*bt_del };
	clx_bt = std::make_shared<CtrlListX>(tmp);
	tmp = {};
	spy = std::make_shared<SeparatorY>(400);
}
void ClyCon::Update(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	auto* selectedConnection = simulationApp.connectionSelecting;
	if (con != selectedConnection) {
		con = selectedConnection;
		for (auto c : cs) { c->Discard(app); }
		if (con) { cs = { &*rtb, &*clx_bt, &*spy }; }
		else { cs = {}; }
	}
	CtrlListY::Update(app);
}
void ClyCon::Discard(App& app) {
	CtrlListY::Discard(app);
	con = NULL; cs = {};
}

void RLbCon::Sync(App& app) {
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

FPnCon::FPnCon(App& app) : FPnLan(app, 400, 600, L"fpn_con") {
	vector<Control*> tmp;
	cly_con = std::make_shared<ClyCon>(); rlb_con = std::make_shared<RLbCon>();
	tmp = { &*cly_con, &*rlb_con };
	cly = std::make_shared<CtrlListY>(tmp);
	c = &*cly;
	Init(app);
}



