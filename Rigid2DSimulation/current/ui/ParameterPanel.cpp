#include "ParameterPanel.h"
#include "SimulationApp.h"
#include "SceneParameter.h"

TbParNm::TbParNm(int id) : id(id), TextBox(270) {};
void TbParNm::Sync(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); str = simulationApp.parameters[id]->name; }
void TbParNm::OnDone(App& app) const { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.parameters[id]->name = str; }
BtDelPar::BtDelPar(int id) : id(id), LocalizedButton(80, L"bt_del_par") {}
void BtDelPar::OnClick(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.parameters[id]->isMarkedForDeletion = true; }
DbXParVal::DbXParVal(int id) : id(id), HorizontalDragBar(270) {}
void DbXParVal::Sync(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); set_nx_rel(simulationApp.parameters[id]->value); }
void DbXParVal::Upload(App& app) const { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.parameters[id]->value = uval(); }
TbParVal::TbParVal(int id) : id(id), TextBox(80) {};
void TbParVal::Sync(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); str = FormatFixed2(simulationApp.parameters[id]->value); }
void TbParVal::OnDone(App& app) const { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.parameters[id]->value = wtof(str); }

ClyPar::ClyPar(int id) {
	vector<Control*> tmp;
	tb_nm = std::make_shared<TbParNm>(id); bt_del = std::make_shared<BtDelPar>(id);
	tmp = { &*tb_nm, &*bt_del };
	clx_nm = std::make_shared<HorizontalControlList>(tmp);
	tmp = {};
	dbx_val = std::make_shared<DbXParVal>(id); tb_val = std::make_shared<TbParVal>(id);
	tmp = { &*dbx_val, &*tb_val };
	clx_val = std::make_shared<HorizontalControlList>(tmp);
	tmp = {};
	spy = std::make_shared<HorizontalSeparator>(400);
	cs = { &*clx_nm, &*clx_val, &*spy };
}
void ClyPars::Update(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	cs.clear();
	for (int i = (mbs.size()), iEnd = int(simulationApp.parameters.size()); i < iEnd; ++i) { mbs.push_back(std::make_shared<ClyPar>(i)); }
	for (int i = (0), iEnd = int(simulationApp.parameters.size()); i < iEnd; ++i) { cs.push_back(&*mbs[i]); }
	for (int i = (simulationApp.parameters.size()), iEnd = int(mbs.size()); i < iEnd; ++i) { mbs[i]->Discard(app); }
	VerticalControlList::Update(app);
}

BtAddPar::BtAddPar() : LocalizedButton(160, L"bt_add_par") {}
void BtAddPar::OnClick(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.parameters.push_back(std::make_shared<SceneParameter>(simulationApp.getLocalizedString(L"unnamed_par"))); }

FPnPar::FPnPar(App& app) : LocalizedFloatPanel(app, 400, 600, L"fpn_par") {
	vector<Control*> tmp;
	cly_pars = std::make_shared<ClyPars>();
	bt_add_par = std::make_shared<BtAddPar>();
	tmp = { &*cly_pars, &*bt_add_par };
	cly = std::make_shared<VerticalControlList>(tmp);
	c = &*cly;
	Init(app);
}



