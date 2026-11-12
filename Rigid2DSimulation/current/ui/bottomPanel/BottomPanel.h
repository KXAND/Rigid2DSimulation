#pragma once

#include "ControlContainer.h"
#include "Label.h"
#include "Panel.h"

class BottomPanel : public Panel
{
public:
    BottomPanel(App& app);
    void Update(App& app) override;

private:
    Label* frameRateLabel = nullptr;
    Label* timeLabel = nullptr;
    Label* mousePositionLabel = nullptr;
};
