#include "BodyPanel.h"
#include "SimulationApp.h"
#include "Body.h"
#include "Connection.h"

RTbBody::RTbBody() : RichTextbox(360, 400) {}
void RTbBody::Sync(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); str = simulationApp.bodySelecting->temporaryScriptText; }
void RTbBody::Upload(App& app) const { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.bodySelecting->temporaryScriptText = str; }

BtBodyOK::BtBodyOK() : BtLan(70, L"bt_body_ok") {}
void BtBodyOK::OnClick(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	auto& selectedBody = *simulationApp.bodySelecting;
	selectedBody.scriptText = selectedBody.temporaryScriptText;
	selectedBody.RefreshFromScript(simulationApp);
}
BtBodyDel::BtBodyDel() : BtLan(70, L"bt_body_del") {}
void BtBodyDel::OnClick(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.bodySelecting->isMarkedForDeletion = true; simulationApp.isSceneChanged = true; }


LbBodyCon::LbBodyCon(int id) : id(id), Label() {
	fixed_w = true; w = 270;
}
void LbBodyCon::Sync(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	auto* connection = simulationApp.bodySelecting->connections[id];
	wstring type;
	switch (connection->type) {
	case CON_ROPE: type = simulationApp.getLocalizedString(L"conn_rope"); break;
	case CON_LINK: type = simulationApp.getLocalizedString(L"conn_link"); break;
	case CON_SPRING: type = simulationApp.getLocalizedString(L"conn_spring"); break;
	case CON_CORD: type = simulationApp.getLocalizedString(L"conn_cord"); break;
	}
	txt = simulationApp.getLocalizedString(L"conn") + std::to_wstring(id) + L": " + type;
}

CbBodyCon::CbBodyCon(int id) : id(id), CbLan(L"cb_body_con") {}
void CbBodyCon::Sync(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); checked = (simulationApp.connectionSelecting == simulationApp.bodySelecting->connections[id]); }
void CbBodyCon::Upload(App& app) const { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.connectionSelecting = checked ? simulationApp.bodySelecting->connections[id] : NULL; }

ClXBodyCon::ClXBodyCon(int id) {
	cb = std::make_shared<CbBodyCon>(id); lb = std::make_shared<LbBodyCon>(id);
	cs = { &*lb, &*cb };
}
void ClYBodyCons::Update(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	auto& selectedBody = *simulationApp.bodySelecting;
	for (int i = (mbs.size()), iEnd = int(selectedBody.connections.size()); i < iEnd; ++i) {
		mbs.push_back(std::make_shared<ClXBodyCon>(i));
	}
	cs.clear();
	for (int i = (0), iEnd = int(selectedBody.connections.size()); i < iEnd; ++i) {
		cs.push_back(&*mbs[i]);
	}
	for (int i = (selectedBody.connections.size()), iEnd = int(mbs.size()); i < iEnd; ++i) {
		mbs[i]->Discard(app);
	}
	CtrlListY::Update(app);
}

ClyBody::ClyBody() {
	vector<Control*> tmp;
	rtb = std::make_shared<RTbBody>();
	bt_ok = std::make_shared<BtBodyOK>(); bt_del = std::make_shared<BtBodyDel>();
	tmp = { &*bt_ok, &*bt_del };
	clx_bt = std::make_shared<CtrlListX>(tmp);
	tmp = {};
	cly_cons = std::make_shared<ClYBodyCons>(); spy = std::make_shared<SeparatorY>(400);
}
void ClyBody::Update(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	auto* selectedBody = simulationApp.bodySelecting;
	if (bo != selectedBody) {
		bo = selectedBody;
		for (auto c : cs) { c->Discard(app); }
		if (bo) { cs = { &*cly_cons, &*rtb, &*clx_bt, &*spy }; }
		else { cs = {}; }
	}
	CtrlListY::Update(app);
}
void ClyBody::Discard(App& app) {
	CtrlListY::Discard(app);
	bo = NULL; cs = {};
}

void RLbBody::Sync(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	auto* selectedBody = simulationApp.bodySelecting;
	if (!selectedBody) { txt = simulationApp.getLocalizedString(L"body_not_sel"); return; }
	auto& body = *selectedBody;
	txt.clear();

	txt += simulationApp.getLocalizedString(L"body_o") + ToWString(body.position) + L"\n";
	txt += simulationApp.getLocalizedString(L"body_v") + ToWString(body.velocity) + L"\n";
	txt += simulationApp.getLocalizedString(L"body_ang") + FormatFixed2(body.radian) + L"\n";
	txt += simulationApp.getLocalizedString(L"body_v_ang") + FormatFixed2(body.velocityAngular) + L"\n";
	txt += simulationApp.getLocalizedString(L"body_fixed") + std::to_wstring(body.fixed) + L"\n";
	txt += simulationApp.getLocalizedString(L"body_density") + FormatFixed2(body.density) + L"\n";
	txt += simulationApp.getLocalizedString(L"body_charge_density") + FormatFixed2(body.chargeDensity) + L"\n";
	txt += simulationApp.getLocalizedString(L"body_e") + FormatFixed2(body.elasticity) + L"\n";
	txt += simulationApp.getLocalizedString(L"body_mu_s") + FormatFixed2(body.frictionStatic) + L"\n";
	txt += simulationApp.getLocalizedString(L"body_mu_d") + FormatFixed2(body.frictionDynamic) + L"\n";
	txt += simulationApp.getLocalizedString(L"body_damp_v") + FormatFixed2(body.dampCoeff) + L"\n";
	txt += simulationApp.getLocalizedString(L"body_damp_v_ang") + FormatFixed2(body.dampCoeffAngular) + L"\n";
}

FPnBody :: FPnBody(App& app) : FPnLan(app, 400, 600, L"fpn_body") {
	vector<Control*> tmp;
	cly_body = std::make_shared<ClyBody>(); rlb_body = std::make_shared<RLbBody>();
	tmp = { &*cly_body, &*rlb_body };
	cly = std::make_shared<CtrlListY>(tmp);
	c = &*cly;
	Init(app);
}



