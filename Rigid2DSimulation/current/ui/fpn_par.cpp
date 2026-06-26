#include "fpn_par.h"
#include "simulation_app.h"
#include "scene_parameter.h"

TbParNm::TbParNm(int id) : id(id), Textbox(270) {};
void TbParNm::Sync(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); str = simulationApp.pars[id]->nm; }
void TbParNm::OnDone(App& app) const { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.pars[id]->nm = str; }
BtDelPar::BtDelPar(int id) : id(id), BtLan(80, L"bt_del_par") {}
void BtDelPar::OnClick(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.pars[id]->del = true; }
DbXParVal::DbXParVal(int id) : id(id), DragbarX(270) {}
void DbXParVal::Sync(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); set_nx_rel(simulationApp.pars[id]->val); }
void DbXParVal::Upload(App& app) const { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.pars[id]->val = uval(); }
TbParVal::TbParVal(int id) : id(id), Textbox(80) {};
void TbParVal::Sync(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); str = tw2(simulationApp.pars[id]->val); }
void TbParVal::OnDone(App& app) const { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.pars[id]->val = wtof(str); }

ClyPar::ClyPar(int id) {
	vector<Control*> tmp;
	mkp(tb_nm)(id); mkp(bt_del)(id);
	tmp = { &*tb_nm, &*bt_del };
	mkp(clx_nm)(tmp);
	tmp = {};
	mkp(dbx_val)(id); mkp(tb_val)(id);
	tmp = { &*dbx_val, &*tb_val };
	mkp(clx_val)(tmp);
	tmp = {};
	mkp(spy)(400);
	cs = { &*clx_nm, &*clx_val, &*spy };
}
void ClyPars::Update(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	cs.clear();
	rep(i, mbs.size(), simulationApp.pars.size()) { mbs.push_back(msh<ClyPar>(i)); }
	rep(i, 0, simulationApp.pars.size()) { cs.push_back(&*mbs[i]); }
	rep(i, simulationApp.pars.size(), mbs.size()) { mbs[i]->Discard(app); }
	CtrlListY::Update(app);
}

BtAddPar::BtAddPar() : BtLan(160, L"bt_add_par") {}
void BtAddPar::OnClick(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.pars.push_back(msh<SceneParameter>(simulationApp.getLocalizedString(L"unnamed_par"))); }

FPnPar::FPnPar(App& app) : FPnLan(app, 400, 600, L"fpn_par") {
	vector<Control*> tmp;
	mkp(cly_pars)();
	mkp(bt_add_par)();
	tmp = { &*cly_pars, &*bt_add_par };
	mkp(cly)(tmp);
	c = &*cly;
	Init(app);
}



