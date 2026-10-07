#pragma once

#include "Label.h"

class FrameRateLabel : public Label
{
public:
    FrameRateLabel();
    void Sync(App& app) override;
};

class TimeLabel : public Label
{
public:
    TimeLabel();
    void Sync(App& app) override;
};

class MousePositionLabel : public Label
{
public:
    MousePositionLabel();
    void Sync(App& app) override;
};
