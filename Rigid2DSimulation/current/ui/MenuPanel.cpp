#include "MenuPanel.h"
#include "UserInterface.h"
#include "SimulationApp.h"
#include "GlobalPanel.h"
#include "BodyPanel.h"
#include "ConnectionPanel.h"
#include "CommandPanel.h"
#include "ParameterPanel.h"
#include "LanguagePanel.h"
#include "AboutPanel.h"
#include "Creator.h"

PauseButton::PauseButton() : Button(150) {}
void PauseButton::Update(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    text = simulationApp.isPaused ? simulationApp.GetLocalizedString(L"start") : simulationApp.GetLocalizedString(L"pause");
    Button::Update(app);
}
void PauseButton::OnClick(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    simulationApp.isPaused = !simulationApp.isPaused;
}

ResetButton::ResetButton() : LocalizedButton(150, L"resetButton") {}
void ResetButton::OnClick(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    simulationApp.Reset();
}

MuteButton::MuteButton() : Button(150) {}
void MuteButton::Update(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    text = simulationApp.isMuted ? simulationApp.GetLocalizedString(L"unmute") : simulationApp.GetLocalizedString(L"mute");
    Button::Update(app);
}
void MuteButton::OnClick(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    simulationApp.isMuted = !simulationApp.isMuted;
}

ModeButton::ModeButton() : Button(150) {}
void ModeButton::Update(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    switch (simulationApp.mode)
    {
    case kDragMode:
        text = simulationApp.GetLocalizedString(L"dragMode");
        break;
    case kSelectMode:
        text = simulationApp.GetLocalizedString(L"selectMode");
        break;
    case kCreateMode:
        text = simulationApp.GetLocalizedString(L"createMode");
        break;
    case kDeleteMode:
        text = simulationApp.GetLocalizedString(L"deleteMode");
        break;
    }
    Button::Update(app);
}
void ModeButton::OnClick(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    simulationApp.creator->Discard(simulationApp);
    simulationApp.mode = (simulationApp.mode + 1) % 4;
}

DisplayButton::DisplayButton() : Button(150) {}
void DisplayButton::Update(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    switch (simulationApp.display)
    {
    case kColorDisplay:
        text = simulationApp.GetLocalizedString(L"colorDisplay");
        break;
    case kEnergyDisplay:
        text = simulationApp.GetLocalizedString(L"energyDisplay");
        break;
    case kChargesDisplay:
        text = simulationApp.GetLocalizedString(L"chargeDisplay");
        break;
    case kNoDisplay:
        text = simulationApp.GetLocalizedString(L"noDisplay");
        break;
    }
    Button::Update(app);
}
void DisplayButton::OnClick(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    simulationApp.display = (simulationApp.display + 1) % 4;
}

MenuPanel::MenuPanel(App& app) : Panel(Panel::CreateMenuPanel(app))
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    vector<Control*> controls;
    pauseButton = std::make_shared<PauseButton>();
    resetButton = std::make_shared<ResetButton>();
    muteButton = std::make_shared<MuteButton>();
    globalPanelButton = std::make_shared<ShowFloatPanelButton>(&*simulationApp.userInterface->globalPanel);
    bodyPanelButton = std::make_shared<ShowFloatPanelButton>(&*simulationApp.userInterface->bodyPanel);
    connectionPanelButton = std::make_shared<ShowFloatPanelButton>(&*simulationApp.userInterface->connectionPanel);
    modeButton = std::make_shared<ModeButton>();
    displayButton = std::make_shared<DisplayButton>();
    commandPanelButton = std::make_shared<ShowFloatPanelButton>(&*simulationApp.userInterface->commandPanel);
    parameterPanelButton = std::make_shared<ShowFloatPanelButton>(&*simulationApp.userInterface->parameterPanel);
    languagePanelButton = std::make_shared<ShowFloatPanelButton>(&*simulationApp.userInterface->languagePanel);
    aboutPanelButton = std::make_shared<ShowFloatPanelButton>(&*simulationApp.userInterface->aboutPanel);
    controls = {&*pauseButton, &*resetButton, &*muteButton, &*globalPanelButton,
                &*bodyPanelButton, &*connectionPanelButton, &*modeButton, &*displayButton,
                &*commandPanelButton, &*parameterPanelButton, &*languagePanelButton, &*aboutPanelButton};
    this->controls = std::make_shared<VerticalControlList>(controls);
    contentControl = &*this->controls;
    Init(app);
}
