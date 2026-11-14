#include "MenuPanel.h"

#include <memory>
#include <utility>

#include "Button.h"
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

MenuPanel::MenuPanel(App& app) : Panel(Panel::CreateMenuPanel(app.screen.Rect()))
{
    auto layout = std::make_unique<VerticalStack>();
    BuildSimulationButtons(*layout);
    BuildScenePanelButtons(*layout, app);
    BuildModeButtons(*layout);
    BuildUtilityPanelButtons(*layout, app);

    SetContent(std::move(layout));
}

void MenuPanel::BuildSimulationButtons(VerticalStack& layout)
{
    auto& pauseButton = layout.AddButton(150, L"按钮", [](App& app)
    {
        static_cast<SimulationApp&>(app).TogglePausing();
    });
    pauseButton.textProvider = [](App& app)
    {
        auto& simulationApp = static_cast<SimulationApp&>(app);
        return app.GetLocalizedString(simulationApp.isPaused ? L"start" : L"pause");
    };

    layout.AddLocalizedButton(150, L"resetButton", [](App& app)
    {
        static_cast<SimulationApp&>(app).Reset();
    });

    auto& muteButton = layout.AddButton(150, L"按钮", [](App& app)
    {
        auto& simulationApp = static_cast<SimulationApp&>(app);
        simulationApp.music.isMuted = !simulationApp.music.isMuted;
    });
    muteButton.textProvider = [](App& app)
    {
        auto& simulationApp = static_cast<SimulationApp&>(app);
        return app.GetLocalizedString(simulationApp.music.isMuted ? L"unmute" : L"mute");
    };
}

void MenuPanel::BuildModeButtons(VerticalStack& layout)
{
    modeButton = &layout.AddButton(150, L"按钮", [](App& app)
    {
        auto& simulationApp = static_cast<SimulationApp&>(app);
        simulationApp.editor.CycleMode(simulationApp.inputOwners);
    });
    modeButton->textProvider = GetInteractionModeText;

    displayButton = &layout.AddButton(150, L"按钮", [](App& app)
    {
        auto& simulationApp = static_cast<SimulationApp&>(app);
        simulationApp.sceneRenderer.display = (simulationApp.sceneRenderer.display + 1) % 4;
    });
    displayButton->textProvider = GetDisplayModeText;
}

void MenuPanel::BuildScenePanelButtons(VerticalStack& layout, App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    layout.AddPanelButton(*simulationApp.userInterface->globalPanel);
    layout.AddPanelButton(*simulationApp.userInterface->creationPanel);
    layout.AddPanelButton(*simulationApp.userInterface->bodyPanel);
    layout.AddPanelButton(*simulationApp.userInterface->connectionPanel);
}

void MenuPanel::BuildUtilityPanelButtons(VerticalStack& layout, App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    layout.AddPanelButton(*simulationApp.userInterface->commandPanel);
    layout.AddPanelButton(*simulationApp.userInterface->parameterPanel);
    layout.AddPanelButton(*simulationApp.userInterface->languagePanel);
    layout.AddPanelButton(*simulationApp.userInterface->aboutPanel);
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
