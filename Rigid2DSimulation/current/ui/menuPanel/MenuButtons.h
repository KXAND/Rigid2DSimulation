#pragma once

#include "Button.h"

class SimulationPauseButton : public Button
{
public:
    SimulationPauseButton();
    void Update(App& app) override;
};

class AudioMuteButton : public Button
{
public:
    AudioMuteButton();
    void Update(App& app) override;
};

class InteractionModeButton : public Button
{
public:
    InteractionModeButton();
    void Update(App& app) override;
};

class DisplayModeButton : public Button
{
public:
    DisplayModeButton();
    void Update(App& app) override;
};
