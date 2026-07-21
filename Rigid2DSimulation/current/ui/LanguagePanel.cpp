#include "LanguagePanel.h"
#include "UserInterface.h"
#include "SimulationApp.h"


ChineseLanguageButton::ChineseLanguageButton() : Button(150) { text = L"ÖÐÎÄ"; }
void ChineseLanguageButton::OnClick(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.languageIdx = kChineseLanguage; simulationApp.userInterface->languagePanel->Hide(app); }
EnglishLanguageButton::EnglishLanguageButton() : Button(150) { text = L"English"; }
void EnglishLanguageButton::OnClick(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.languageIdx = kEnglishLanguage; simulationApp.userInterface->languagePanel->Hide(app); }

LanguagePanel::LanguagePanel(App& app) : LocalizedFloatPanel(app, 200, 200, L"fpn_lang") {
	vector<Control*> controls; shown = true;
	chineseButton = std::make_shared<ChineseLanguageButton>();
	englishButton = std::make_shared<EnglishLanguageButton>();
	controls = { &*chineseButton, &*englishButton };
	this->controls = std::make_shared<VerticalControlList>(controls);
	contentControl = &*this->controls;
	Init(app);
}



