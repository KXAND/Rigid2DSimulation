#include "LanguagePanel.h"

#include <memory>
#include <utility>

#include "SimulationApp.h"
#include "UserInterface.h"

LanguagePanel::LanguagePanel(App& app) : LocalizedFloatPanel(app.screen.Rect(), 200, 200, L"languagePanelTitle")
{
    shown = true;
    auto layout = std::make_unique<VerticalStack>();
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

    SetContent(std::move(layout));
}
