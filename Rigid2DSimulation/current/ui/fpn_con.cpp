#include "fpn_con.h"
#include "simulation_app.h"
#include "connection.h"

RTbCon::RTbCon() : RichTextbox(360, 400) {}
void RTbCon::Sync(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); str = simulationApp.connectionSelecting->temporaryScriptText; }
void RTbCon::Upload(App& app) const { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.connectionSelecting->temporaryScriptText = str; }

BtConOK::BtConOK() : BtLan(70, L"bt_con_ok") {}
void BtConOK::OnClick(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	auto& selectedConnection = *simulationApp.connectionSelecting;
	selectedConnection.scriptText = selectedConnection.temporaryScriptText;
	selectedConnection.refresh(simulationApp);
}
BtConDel::BtConDel() : BtLan(70, L"bt_con_del") {}
void BtConDel::OnClick(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.connectionSelecting->del = true; simulationApp.isSceneChanged = true; }

ClyCon::ClyCon() {
	vector<Control*> tmp;
	mkp(rtb)(); 
	mkp(bt_ok)(); mkp(bt_del)();
	tmp = { &*bt_ok, &*bt_del };
	mkp(clx_bt)(tmp);
	tmp = {};
	mkp(spy)(400);
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
	txt += simulationApp.getLocalizedString(L"conn_len") + tw2(connection.len) + L"\n";
	txt += simulationApp.getLocalizedString(L"conn_hooke") + tw2(connection.hooke) + L"\n";
}

FPnCon::FPnCon(App& app) : FPnLan(app, 400, 600, L"fpn_con") {
	vector<Control*> tmp;
	mkp(cly_con)(); mkp(rlb_con)();
	tmp = { &*cly_con, &*rlb_con };
	mkp(cly)(tmp);
	c = &*cly;
	Init(app);
}



