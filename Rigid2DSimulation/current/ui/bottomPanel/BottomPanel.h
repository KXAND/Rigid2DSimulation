#pragma once

#include <memory>

#include "BottomControls.h"
#include "ControlList.h"
#include "Panel.h"

class BottomPanel : public Panel
{
public:
    std::shared_ptr<FrameRateLabel> frameRateLabel;
    std::shared_ptr<TimeLabel> timeLabel;
    std::shared_ptr<MousePositionLabel> mousePositionLabel;
    std::shared_ptr<HorizontalControlList> controls;
    BottomPanel(App& app);
};
