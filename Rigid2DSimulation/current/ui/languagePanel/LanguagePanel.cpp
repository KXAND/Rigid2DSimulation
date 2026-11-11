#include "LanguagePanel.h"

#include <memory>

#include "SimulationApp.h"
#include "UserInterface.h"

LanguagePanel::LanguagePanel(App& app) : LocalizedFloatPanel(app, 200, 200, L"languagePanelTitle")
{
    shown = true;
    layout = std::make_unique<VerticalControlList>();
    layout->AddButton(150, L"中文", [](App& app)
    {
        auto& simulationApp = static_cast<SimulationApp&>(app);
        simulationApp.languageIdx = kChineseLanguage;
        simulationApp.userInterface->languagePanel->Hide(app);
    });

    layout->AddButton(150, L"English", [](App& app)
    {
        auto& simulationApp = static_cast<SimulationApp&>(app);
        simulationApp.languageIdx = kEnglishLanguage;
        simulationApp.userInterface->languagePanel->Hide(app);
    });

    contentControl = layout.get();
    Init(app);
}
