#include "LanguagePanel.h"

#include <memory>
#include <vector>

#include "SimulationApp.h"
#include "UserInterface.h"

LanguagePanel::LanguagePanel(App& app) : LocalizedFloatPanel(app, 200, 200, L"languagePanelTitle")
{
    vector<Control*> controls;
    shown = true;
    chineseButton = std::make_shared<Button>(150);
    chineseButton->text = L"中文";
    chineseButton->onClick = [](App& app)
    {
        auto& simulationApp = static_cast<SimulationApp&>(app);
        simulationApp.languageIdx = kChineseLanguage;
        simulationApp.userInterface->languagePanel->Hide(app);
    };
    englishButton = std::make_shared<Button>(150);
    englishButton->text = L"English";
    englishButton->onClick = [](App& app)
    {
        auto& simulationApp = static_cast<SimulationApp&>(app);
        simulationApp.languageIdx = kEnglishLanguage;
        simulationApp.userInterface->languagePanel->Hide(app);
    };
    controls = {&*chineseButton, &*englishButton};
    this->controls = std::make_shared<VerticalControlList>(controls);
    contentControl = &*this->controls;
    Init(app);
}
