#pragma once

#include <memory>
#include <string>

#include "Button.h"
#include "ControlList.h"
#include "Panel.h"

class MenuPanel : public Panel
{
public:
    std::shared_ptr<Button> pauseButton;
    std::shared_ptr<LocalizedButton> resetButton;
    std::shared_ptr<Button> muteButton;
    std::shared_ptr<ShowFloatPanelButton> globalPanelButton;
    std::shared_ptr<ShowFloatPanelButton> creationPanelButton;
    std::shared_ptr<ShowFloatPanelButton> bodyPanelButton;
    std::shared_ptr<ShowFloatPanelButton> connectionPanelButton;
    std::shared_ptr<Button> modeButton;
    std::shared_ptr<Button> displayButton;
    std::shared_ptr<ShowFloatPanelButton> commandPanelButton;
    std::shared_ptr<ShowFloatPanelButton> parameterPanelButton;
    std::shared_ptr<ShowFloatPanelButton> languagePanelButton;
    std::shared_ptr<ShowFloatPanelButton> aboutPanelButton;
    std::shared_ptr<VerticalControlList> layout;
    MenuPanel(App& app);

private:
    void BuildSimulationButtons();
    void BuildModeButtons();
    void BuildPanelButtons(App& app);
    static std::wstring GetInteractionModeText(App& app);
    static std::wstring GetDisplayModeText(App& app);
};
