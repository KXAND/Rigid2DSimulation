#include "fpn_par.h"
#include "simulation_app.h"
#include "scene_parameter.h"

TbParNm::TbParNm(int id) : id(id), Textbox(270) {};
void TbParNm::Sync(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); str = simulationApp.parameters[id]->name; }
void TbParNm::OnDone(App& app) const { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.parameters[id]->name = str; }
BtDelPar::BtDelPar(int id) : id(id), BtLan(80, L"bt_del_par") {}
void BtDelPar::OnClick(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.parameters[id]->isMarkedForDeletion = true; }
DbXParVal::DbXParVal(int id) : id(id), DragbarX(270) {}
void DbXParVal::Sync(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); set_nx_rel(simulationApp.parameters[id]->value); }
void DbXParVal::Upload(App& app) const { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.parameters[id]->value = uval(); }
TbParVal::TbParVal(int id) : id(id), Textbox(80) {};
void TbParVal::Sync(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); str = tw2(simulationApp.parameters[id]->value); }
void TbParVal::OnDone(App& app) const { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.parameters[id]->value = wtof(str); }

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
	for (int i = (mbs.size()), iEnd = int(simulationApp.parameters.size()); i < iEnd; ++i) { mbs.push_back(msh<ClyPar>(i)); }
	for (int i = (0), iEnd = int(simulationApp.parameters.size()); i < iEnd; ++i) { cs.push_back(&*mbs[i]); }
	for (int i = (simulationApp.parameters.size()), iEnd = int(mbs.size()); i < iEnd; ++i) { mbs[i]->Discard(app); }
	CtrlListY::Update(app);
}

BtAddPar::BtAddPar() : BtLan(160, L"bt_add_par") {}
void BtAddPar::OnClick(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.parameters.push_back(msh<SceneParameter>(simulationApp.getLocalizedString(L"unnamed_par"))); }

FPnPar::FPnPar(App& app) : FPnLan(app, 400, 600, L"fpn_par") {
	vector<Control*> tmp;
	mkp(cly_pars)();
	mkp(bt_add_par)();
	tmp = { &*cly_pars, &*bt_add_par };
	mkp(cly)(tmp);
	c = &*cly;
	Init(app);
}



