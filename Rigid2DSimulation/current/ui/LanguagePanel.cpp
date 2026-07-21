#include "LanguagePanel.h"
#include "UserInterface.h"
#include "SimulationApp.h"


ChineseLanguageButton::ChineseLanguageButton() : Button(150) { text = L"ÖÐÎÄ"; }
void ChineseLanguageButton::OnClick(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.languageIdx = LAN_CHI; simulationApp.userInterface->languagePanel->Hide(app); }
EnglishLanguageButton::EnglishLanguageButton() : Button(150) { text = L"English"; }
void EnglishLanguageButton::OnClick(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.languageIdx = LAN_ENG; simulationApp.userInterface->languagePanel->Hide(app); }

LanguagePanel::LanguagePanel(App& app) : LocalizedFloatPanel(app, 200, 200, L"fpn_lang") {
	vector<Control*> tmp; shown = true;
	chineseButton = std::make_shared<ChineseLanguageButton>();
	englishButton = std::make_shared<EnglishLanguageButton>();
	tmp = { &*chineseButton, &*englishButton };
	controls = std::make_shared<VerticalControlList>(tmp);
	c = &*controls;
	Init(app);
}



