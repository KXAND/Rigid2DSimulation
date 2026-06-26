#include "fpn_body.h"
#include "simulation_app.h"
#include "body.h"
#include "connection.h"

RTbBody::RTbBody() : RichTextbox(360, 400) {}
void RTbBody::Sync(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); str = simulationApp.bodySelecting->temporaryScriptText; }
void RTbBody::Upload(App& app) const { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.bodySelecting->temporaryScriptText = str; }

BtBodyOK::BtBodyOK() : BtLan(70, L"bt_body_ok") {}
void BtBodyOK::OnClick(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	auto& selectedBody = *simulationApp.bodySelecting;
	selectedBody.scriptText = selectedBody.temporaryScriptText;
	selectedBody.refresh(simulationApp);
}
BtBodyDel::BtBodyDel() : BtLan(70, L"bt_body_del") {}
void BtBodyDel::OnClick(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.bodySelecting->del = true; simulationApp.isSceneChanged = true; }


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
	txt = simulationApp.getLocalizedString(L"conn") + tw(id) + L": " + type;
}

CbBodyCon::CbBodyCon(int id) : id(id), CbLan(L"cb_body_con") {}
void CbBodyCon::Sync(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); checked = (simulationApp.connectionSelecting == simulationApp.bodySelecting->connections[id]); }
void CbBodyCon::Upload(App& app) const { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.connectionSelecting = checked ? simulationApp.bodySelecting->connections[id] : NULL; }

ClXBodyCon::ClXBodyCon(int id) {
	mkp(cb)(id); mkp(lb)(id);
	cs = { &*lb, &*cb };
}
void ClYBodyCons::Update(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	auto& selectedBody = *simulationApp.bodySelecting;
	rep(i, mbs.size(), selectedBody.connections.size()) {
		mbs.push_back(msh<ClXBodyCon>(i));
	}
	cs.clear();
	rep(i, 0, selectedBody.connections.size()) {
		cs.push_back(&*mbs[i]);
	}
	rep(i, selectedBody.connections.size(), mbs.size()) {
		mbs[i]->Discard(app);
	}
	CtrlListY::Update(app);
}

ClyBody::ClyBody() {
	vector<Control*> tmp;
	mkp(rtb)(); 
	mkp(bt_ok)(); mkp(bt_del)();
	tmp = { &*bt_ok, &*bt_del };
	mkp(clx_bt)(tmp);
	tmp = {};
	mkp(cly_cons)(); mkp(spy)(400);
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
	
	txt += simulationApp.getLocalizedString(L"body_o") + toWstring(body.position) + L"\n";
	txt += simulationApp.getLocalizedString(L"body_v") + toWstring(body.velocity) + L"\n";
	txt += simulationApp.getLocalizedString(L"body_ang") + tw2(body.radian) + L"\n";
	txt += simulationApp.getLocalizedString(L"body_v_ang") + tw2(body.velocityAngular) + L"\n";
	txt += simulationApp.getLocalizedString(L"body_fixed") + tw(body.fixed) + L"\n";
	txt += simulationApp.getLocalizedString(L"body_density") + tw2(body.density) + L"\n";
	txt += simulationApp.getLocalizedString(L"body_charge_density") + tw2(body.chargeDensity) + L"\n";
	txt += simulationApp.getLocalizedString(L"body_e") + tw2(body.elasticity) + L"\n";
	txt += simulationApp.getLocalizedString(L"body_mu_s") + tw2(body.frictionStatic) + L"\n";
	txt += simulationApp.getLocalizedString(L"body_mu_d") + tw2(body.frictionDynamic) + L"\n";
	txt += simulationApp.getLocalizedString(L"body_damp_v") + tw2(body.dampCoeff) + L"\n";
	txt += simulationApp.getLocalizedString(L"body_damp_v_ang") + tw2(body.dampCoeffAngular) + L"\n";
}

FPnBody :: FPnBody(App& app) : FPnLan(app, 400, 600, L"fpn_body") {
	vector<Control*> tmp;
	mkp(cly_body)(); mkp(rlb_body)();
	tmp = { &*cly_body, &*rlb_body };
	mkp(cly)(tmp);
	c = &*cly;
	Init(app);
}



