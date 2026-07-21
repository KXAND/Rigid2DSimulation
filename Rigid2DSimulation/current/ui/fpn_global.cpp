#include "fpn_global.h"
#include "ui.h"
#include "SimulationApp.h"
#include "creator.h"


BtSave::BtSave() : BtLan(80, L"bt_save") {}
void BtSave::OnClick(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.Save(simulationApp.ui->fpn_global->tb_save->str); }
BtLoad::BtLoad() : BtLan(80, L"bt_load") {}
void BtLoad::OnClick(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.Load(simulationApp.ui->fpn_global->tb_load->str); }

void RLbGlobal::Sync(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	txt = L"";
	txt += simulationApp.getLocalizedString(L"vol") + FormatFixed2(simulationApp.vol) + L"\n";
	txt += simulationApp.getLocalizedString(L"n_step") + std::to_wstring(simulationApp.stepNum) + L"\n";
	txt += simulationApp.getLocalizedString(L"s_grid") + FormatFixed2(simulationApp.gridSize) + L"\n";
	txt += simulationApp.getLocalizedString(L"max_real_dt") + FormatFixed2(simulationApp.maxRealDeltaTime) + L"\n";
	txt += simulationApp.getLocalizedString(L"tl_scene") + ToWString(simulationApp.rectScene.topLeftPosition) + L"\n";
	txt += simulationApp.getLocalizedString(L"w_scene") + std::to_wstring(simulationApp.rectScene.w) + L"\n";
	txt += simulationApp.getLocalizedString(L"h_scene") + std::to_wstring(simulationApp.rectScene.h) + L"\n";
	txt += simulationApp.getLocalizedString(L"gravity") + ToWString(simulationApp.gravity) + L"\n";
	txt += simulationApp.getLocalizedString(L"electrostatic") + std::to_wstring(simulationApp.hasElectrostatic) + L"\n";
	txt += simulationApp.getLocalizedString(L"coulomb") + FormatFixed2(simulationApp.coulomb) + L"\n";

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
	tb_save = std::make_shared<Textbox>(270); bt_save = std::make_shared<BtSave>();
	tmp = { &*tb_save, &*bt_save };
	clx_save = std::make_shared<CtrlListX>(tmp);
	tmp = {};
	tb_load = std::make_shared<Textbox>(270); bt_load = std::make_shared<BtLoad>();
	tmp = { &*tb_load, &*bt_load };
	clx_load = std::make_shared<CtrlListX>(tmp);
	tmp = {};
	spy = std::make_shared<SeparatorY>(400); rlb_global = std::make_shared<RLbGlobal>();
	tmp = { &*clx_save, &*clx_load, &*spy, &*rlb_global };
	cly = std::make_shared<CtrlListY>(tmp);
	c = &*cly;
	Init(app);
}



