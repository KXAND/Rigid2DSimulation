#include "LanguagePanel.h"
#include "UserInterface.h"
#include "SimulationApp.h"


ChineseLanguageButton::ChineseLanguageButton() : Button(150) { txt = L"ÖÐÎÄ"; }
void ChineseLanguageButton::OnClick(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.languageIdx = LAN_CHI; simulationApp.userInterface->fpn_lang->hide(app); }
EnglishLanguageButton::EnglishLanguageButton() : Button(150) { txt = L"English"; }
void EnglishLanguageButton::OnClick(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.languageIdx = LAN_ENG; simulationApp.userInterface->fpn_lang->hide(app); }

LanguagePanel::LanguagePanel(App& app) : LocalizedFloatPanel(app, 200, 200, L"fpn_lang") {
	vector<Control*> tmp; shown = true;
	bt_chi = std::make_shared<ChineseLanguageButton>();
	bt_eng = std::make_shared<EnglishLanguageButton>();
	tmp = { &*bt_chi, &*bt_eng };
	cly = std::make_shared<VerticalControlList>(tmp);
	c = &*cly;
	Init(app);
}



