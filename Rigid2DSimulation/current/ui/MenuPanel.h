#pragma once
#include "Panel.h"
#include "Button.h"
#include "ControlList.h"

struct PauseButton : Button
{
    PauseButton();
    void Update(App& app) override;
    void OnClick(App& app) override;
};
struct ResetButton : LocalizedButton
{
    ResetButton();
    void OnClick(App& app) override;
};
struct MuteButton : Button
{
    MuteButton();
    void Update(App& app) override;
    void OnClick(App& app) override;
};
struct ModeButton : Button
{
    ModeButton();
    void Update(App& app) override;
    void OnClick(App& app) override;
};
struct DisplayButton : Button
{
    DisplayButton();
    void Update(App& app) override;
    void OnClick(App& app) override;
};

struct MenuPanel : Panel
{
    std::shared_ptr<PauseButton> pauseButton;
    std::shared_ptr<ResetButton> resetButton;
    std::shared_ptr<MuteButton> muteButton;
    std::shared_ptr<ShowFloatPanelButton> globalPanelButton;
    std::shared_ptr<ShowFloatPanelButton> bodyPanelButton;
    std::shared_ptr<ShowFloatPanelButton> connectionPanelButton;
    std::shared_ptr<ModeButton> modeButton;
    std::shared_ptr<DisplayButton> displayButton;
    std::shared_ptr<ShowFloatPanelButton> commandPanelButton;
    std::shared_ptr<ShowFloatPanelButton> parameterPanelButton;
    std::shared_ptr<ShowFloatPanelButton> languagePanelButton;
    std::shared_ptr<ShowFloatPanelButton> aboutPanelButton;
    std::shared_ptr<VerticalControlList> controls;
    MenuPanel(App& app);
};
