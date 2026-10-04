#pragma once

#include "Label.h"

struct FrameRateLabel : Label
{
    FrameRateLabel();
    void Sync(App& app) override;
};

struct TimeLabel : Label
{
    TimeLabel();
    void Sync(App& app) override;
};

struct MousePositionLabel : Label
{
    MousePositionLabel();
    void Sync(App& app) override;
};
