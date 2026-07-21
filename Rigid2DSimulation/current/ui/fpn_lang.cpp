#include "fpn_lang.h"
#include "ui.h"
#include "SimulationApp.h"


BtChi::BtChi() : Button(150) { txt = L"ÖÐÎÄ"; }
void BtChi::OnClick(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.languageIdx = LAN_CHI; simulationApp.ui->fpn_lang->hide(app); }
BtEng::BtEng() : Button(150) { txt = L"English"; }
void BtEng::OnClick(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.languageIdx = LAN_ENG; simulationApp.ui->fpn_lang->hide(app); }

FPnLang::FPnLang(App& app) : FPnLan(app, 200, 200, L"fpn_lang") {
	vector<Control*> tmp; shown = true;
	bt_chi = std::make_shared<BtChi>();
	bt_eng = std::make_shared<BtEng>();
	tmp = { &*bt_chi, &*bt_eng };
	cly = std::make_shared<CtrlListY>(tmp);
	c = &*cly;
	Init(app);
}



