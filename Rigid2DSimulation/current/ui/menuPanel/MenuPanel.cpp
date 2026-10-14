#include "MenuPanel.h"

#include <memory>
#include <vector>

#include "SimulationApp.h"
#include "UserInterface.h"
#include "current/ui/aboutPanel/AboutPanel.h"
#include "current/ui/bodyPanel/BodyPanel.h"
#include "current/ui/commandPanel/CommandPanel.h"
#include "current/ui/connectionPanel/ConnectionPanel.h"
#include "current/ui/creationPanel/CreationPanel.h"
#include "current/ui/globalPanel/GlobalPanel.h"
#include "current/ui/languagePanel/LanguagePanel.h"
#include "current/ui/parameterPanel/ParameterPanel.h"

MenuPanel::MenuPanel(App& app) : Panel(Panel::CreateMenuPanel(app))
{
    BuildSimulationButtons();
    BuildPanelButtons(app);
    BuildModeButtons();

    layout = std::make_shared<VerticalControlList>(std::vector<Control*>{
        pauseButton.get(), resetButton.get(), muteButton.get(),
        globalPanelButton.get(), creationPanelButton.get(), bodyPanelButton.get(), connectionPanelButton.get(),
        modeButton.get(), displayButton.get(),
        commandPanelButton.get(), parameterPanelButton.get(), languagePanelButton.get(), aboutPanelButton.get()});
    contentControl = layout.get();
    Init(app);
}

void MenuPanel::BuildSimulationButtons()
{
    pauseButton = std::make_shared<Button>(150);
    pauseButton->textProvider = [](App& app)
    {
        auto& simulationApp = static_cast<SimulationApp&>(app);
        return app.GetLocalizedString(simulationApp.isPaused ? L"start" : L"pause");
    };
    pauseButton->onClick = [](App& app)
    {
        auto& simulationApp = static_cast<SimulationApp&>(app);
        simulationApp.isPaused = !simulationApp.isPaused;
    };

    resetButton = std::make_shared<LocalizedButton>(150, L"resetButton");
    resetButton->onClick = [](App& app)
    {
        static_cast<SimulationApp&>(app).Reset();
    };

    muteButton = std::make_shared<Button>(150);
    muteButton->textProvider = [](App& app)
    {
        auto& simulationApp = static_cast<SimulationApp&>(app);
        return app.GetLocalizedString(simulationApp.isMuted ? L"unmute" : L"mute");
    };
    muteButton->onClick = [](App& app)
    {
        auto& simulationApp = static_cast<SimulationApp&>(app);
        simulationApp.isMuted = !simulationApp.isMuted;
    };
}

void MenuPanel::BuildModeButtons()
{
    modeButton = std::make_shared<Button>(150);
    modeButton->textProvider = GetInteractionModeText;
    modeButton->onClick = [](App& app)
    {
        auto& simulationApp = static_cast<SimulationApp&>(app);
        simulationApp.editor.CycleMode(simulationApp.inputOwners);
    };

    displayButton = std::make_shared<Button>(150);
    displayButton->textProvider = GetDisplayModeText;
    displayButton->onClick = [](App& app)
    {
        auto& simulationApp = static_cast<SimulationApp&>(app);
        simulationApp.sceneRenderer.display = (simulationApp.sceneRenderer.display + 1) % 4;
    };
}

void MenuPanel::BuildPanelButtons(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    globalPanelButton = std::make_shared<ShowFloatPanelButton>(simulationApp.userInterface->globalPanel.get());
    creationPanelButton = std::make_shared<ShowFloatPanelButton>(simulationApp.userInterface->creationPanel.get());
    bodyPanelButton = std::make_shared<ShowFloatPanelButton>(simulationApp.userInterface->bodyPanel.get());
    connectionPanelButton = std::make_shared<ShowFloatPanelButton>(simulationApp.userInterface->connectionPanel.get());
    commandPanelButton = std::make_shared<ShowFloatPanelButton>(simulationApp.userInterface->commandPanel.get());
    parameterPanelButton = std::make_shared<ShowFloatPanelButton>(simulationApp.userInterface->parameterPanel.get());
    languagePanelButton = std::make_shared<ShowFloatPanelButton>(simulationApp.userInterface->languagePanel.get());
    aboutPanelButton = std::make_shared<ShowFloatPanelButton>(simulationApp.userInterface->aboutPanel.get());
}

std::wstring MenuPanel::GetInteractionModeText(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    switch (simulationApp.editor.mode)
    {
    case kDragMode:
        return simulationApp.GetLocalizedString(L"dragMode");
    case kSelectMode:
        return simulationApp.GetLocalizedString(L"selectMode");
    case kCreateMode:
        return simulationApp.GetLocalizedString(L"createMode");
    case kDeleteMode:
        return simulationApp.GetLocalizedString(L"deleteMode");
    }
    // 未知模式沿用原文字，与原来的无 default 分支行为一致。
    return simulationApp.userInterface->menuPanel->modeButton->text;
}

std::wstring MenuPanel::GetDisplayModeText(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    switch (simulationApp.sceneRenderer.display)
    {
    case kColorDisplay:
        return simulationApp.GetLocalizedString(L"colorDisplay");
    case kEnergyDisplay:
        return simulationApp.GetLocalizedString(L"energyDisplay");
    case kChargesDisplay:
        return simulationApp.GetLocalizedString(L"chargeDisplay");
    case kNoDisplay:
        return simulationApp.GetLocalizedString(L"noDisplay");
    }
    // 未知模式沿用原文字，与原来的无 default 分支行为一致。
    return simulationApp.userInterface->menuPanel->displayButton->text;
}
