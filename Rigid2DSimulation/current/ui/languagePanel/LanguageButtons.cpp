#include "LanguageButtons.h"

#include "LanguagePanel.h"
#include "SimulationApp.h"
#include "UserInterface.h"

ChineseLanguageButton::ChineseLanguageButton() : Button(150)
{
    text = L"中文";
}

void ChineseLanguageButton::OnClick(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    simulationApp.languageIdx = kChineseLanguage;
    simulationApp.userInterface->languagePanel->Hide(app);
}

EnglishLanguageButton::EnglishLanguageButton() : Button(150)
{
    text = L"English";
}

void EnglishLanguageButton::OnClick(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    simulationApp.languageIdx = kEnglishLanguage;
    simulationApp.userInterface->languagePanel->Hide(app);
}
