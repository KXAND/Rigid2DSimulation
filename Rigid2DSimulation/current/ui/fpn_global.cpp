#include "fpn_global.h"
#include "ui.h"
#include "simulation_app.h"
#include "creator.h"


BtSave::BtSave() : BtLan(80, L"bt_save") {}
void BtSave::OnClick(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.Save(simulationApp.ui->fpn_global->tb_save->str); }
BtLoad::BtLoad() : BtLan(80, L"bt_load") {}
void BtLoad::OnClick(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.Load(simulationApp.ui->fpn_global->tb_load->str); }

void RLbGlobal::Sync(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	txt = L"";
	txt += simulationApp.getLocalizedString(L"vol") + tw2(simulationApp.vol) + L"\n";
	txt += simulationApp.getLocalizedString(L"n_step") + tw(simulationApp.stepNum) + L"\n";
	txt += simulationApp.getLocalizedString(L"s_grid") + tw2(simulationApp.gridSize) + L"\n";
	txt += simulationApp.getLocalizedString(L"max_real_dt") + tw2(simulationApp.max_real_dt) + L"\n";
	txt += simulationApp.getLocalizedString(L"tl_scene") + tw(simulationApp.rectScene.topLeftPosition) + L"\n";
	txt += simulationApp.getLocalizedString(L"w_scene") + tw(simulationApp.rectScene.w) + L"\n";
	txt += simulationApp.getLocalizedString(L"h_scene") + tw(simulationApp.rectScene.h) + L"\n";
	txt += simulationApp.getLocalizedString(L"gravity") + toWstring(simulationApp.gravity) + L"\n";
	txt += simulationApp.getLocalizedString(L"electrostatic") + tw(simulationApp.hasElectrostatic) + L"\n";
	txt += simulationApp.getLocalizedString(L"coulomb") + tw2(simulationApp.coulomb) + L"\n";

	wstring s;
	switch (simulationApp.creator->mode) {
	case CREATE_BALL: s = simulationApp.getLocalizedString(L"crt_ball"); break;
	case CREATE_BOX: s = simulationApp.getLocalizedString(L"crt_box"); break;
	case CREATE_CONN: s = simulationApp.getLocalizedString(L"crt_conn"); break;
	case CREATE_NAIL: s = simulationApp.getLocalizedString(L"crt_nail"); break;
	case CREATE_PARTICLE: s = simulationApp.getLocalizedString(L"crt_particle"); break;
	case CREATE_PLATE: s = simulationApp.getLocalizedString(L"crt_plate"); break;
	case CREATE_POINT: s = simulationApp.getLocalizedString(L"crt_point"); break;
	}
	txt += simulationApp.getLocalizedString(L"creator_mode") + s + L"\n";
}

FPnGlobal::FPnGlobal(App& app) : FPnLan(app, 400, 600, L"fpn_global") {
	vector<Control*> tmp;
	mkp(tb_save)(270); mkp(bt_save)();
	tmp = { &*tb_save, &*bt_save };
	mkp(clx_save)(tmp);
	tmp = {};
	mkp(tb_load)(270); mkp(bt_load)();
	tmp = { &*tb_load, &*bt_load };
	mkp(clx_load)(tmp);
	tmp = {};
	mkp(spy)(400); mkp(rlb_global)();
	tmp = { &*clx_save, &*clx_load, &*spy, &*rlb_global };
	mkp(cly)(tmp);
	c = &*cly;
	Init(app);
}



