#include "LanguagePanel.h"
#include "UserInterface.h"
#include "SimulationApp.h"


ChineseLanguageButton::ChineseLanguageButton() : Button(150) { txt = L"ÖÐÎÄ"; }
void ChineseLanguageButton::OnClick(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.languageIdx = LAN_CHI; simulationApp.userInterface->languagePanel->hide(app); }
EnglishLanguageButton::EnglishLanguageButton() : Button(150) { txt = L"English"; }
void EnglishLanguageButton::OnClick(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.languageIdx = LAN_ENG; simulationApp.userInterface->languagePanel->hide(app); }

LanguagePanel::LanguagePanel(App& app) : LocalizedFloatPanel(app, 200, 200, L"fpn_lang") {
	vector<Control*> tmp; shown = true;
	chineseButton = std::make_shared<ChineseLanguageButton>();
	englishButton = std::make_shared<EnglishLanguageButton>();
	tmp = { &*chineseButton, &*englishButton };
	controls = std::make_shared<VerticalControlList>(tmp);
	c = &*controls;
	Init(app);
}



