#include "fpn_lang.h"
#include "ui.h"
#include "simulation_app.h"


BtChi::BtChi() : Button(150) { txt = L"ÖÐÎÄ"; }
void BtChi::OnClick(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.languageIdx = LAN_CHI; simulationApp.ui->fpn_lang->hide(app); }
BtEng::BtEng() : Button(150) { txt = L"English"; }
void BtEng::OnClick(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.languageIdx = LAN_ENG; simulationApp.ui->fpn_lang->hide(app); }

FPnLang::FPnLang(App& app) : FPnLan(app, 200, 200, L"fpn_lang") {
	vector<Control*> tmp; shown = true;
	mkp(bt_chi)();
	mkp(bt_eng)();
	tmp = { &*bt_chi, &*bt_eng };
	mkp(cly)(tmp);
	c = &*cly;
	Init(app);
}



