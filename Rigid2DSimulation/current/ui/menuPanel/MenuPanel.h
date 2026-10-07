#pragma once

#include <memory>

#include "Button.h"
#include "ControlList.h"
#include "MenuButtons.h"
#include "Panel.h"

class MenuPanel : public Panel
{
public:
    std::shared_ptr<SimulationPauseButton> pauseButton;
    std::shared_ptr<SceneResetButton> resetButton;
    std::shared_ptr<AudioMuteButton> muteButton;
    std::shared_ptr<ShowFloatPanelButton> globalPanelButton;
    std::shared_ptr<ShowFloatPanelButton> bodyPanelButton;
    std::shared_ptr<ShowFloatPanelButton> connectionPanelButton;
    std::shared_ptr<InteractionModeButton> modeButton;
    std::shared_ptr<DisplayModeButton> displayButton;
    std::shared_ptr<ShowFloatPanelButton> commandPanelButton;
    std::shared_ptr<ShowFloatPanelButton> parameterPanelButton;
    std::shared_ptr<ShowFloatPanelButton> languagePanelButton;
    std::shared_ptr<ShowFloatPanelButton> aboutPanelButton;
    std::shared_ptr<VerticalControlList> controls;
    MenuPanel(App& app);
};
