#pragma once

#include "Button.h"

struct SimulationPauseButton : Button
{
    SimulationPauseButton();
    void Update(App& app) override;
    void OnClick(App& app) override;
};

struct SceneResetButton : LocalizedButton
{
    SceneResetButton();
    void OnClick(App& app) override;
};

struct AudioMuteButton : Button
{
    AudioMuteButton();
    void Update(App& app) override;
    void OnClick(App& app) override;
};

struct InteractionModeButton : Button
{
    InteractionModeButton();
    void Update(App& app) override;
    void OnClick(App& app) override;
};

struct DisplayModeButton : Button
{
    DisplayModeButton();
    void Update(App& app) override;
    void OnClick(App& app) override;
};
