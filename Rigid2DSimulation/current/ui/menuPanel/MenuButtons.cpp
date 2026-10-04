#include "MenuButtons.h"

#include "SimulationApp.h"

SimulationPauseButton::SimulationPauseButton() : Button(150) {}

void SimulationPauseButton::Update(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    text = simulationApp.isPaused ? simulationApp.GetLocalizedString(L"start") : simulationApp.GetLocalizedString(L"pause");
    Button::Update(app);
}

void SimulationPauseButton::OnClick(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    simulationApp.isPaused = !simulationApp.isPaused;
}

SceneResetButton::SceneResetButton() : LocalizedButton(150, L"resetButton") {}

void SceneResetButton::OnClick(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    simulationApp.Reset();
}

AudioMuteButton::AudioMuteButton() : Button(150) {}

void AudioMuteButton::Update(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    text = simulationApp.isMuted ? simulationApp.GetLocalizedString(L"unmute") : simulationApp.GetLocalizedString(L"mute");
    Button::Update(app);
}

void AudioMuteButton::OnClick(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    simulationApp.isMuted = !simulationApp.isMuted;
}

InteractionModeButton::InteractionModeButton() : Button(150) {}

void InteractionModeButton::Update(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    switch (simulationApp.editor.mode)
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

void InteractionModeButton::OnClick(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    simulationApp.editor.CycleMode(simulationApp.inputOwners);
}

DisplayModeButton::DisplayModeButton() : Button(150) {}

void DisplayModeButton::Update(App& app)
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

void DisplayModeButton::OnClick(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    simulationApp.display = (simulationApp.display + 1) % 4;
}
