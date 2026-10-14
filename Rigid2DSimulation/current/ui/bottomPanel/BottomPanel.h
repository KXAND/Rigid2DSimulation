#pragma once

#include <memory>

#include "ControlList.h"
#include "Label.h"
#include "Panel.h"

class BottomPanel : public Panel
{
public:
    std::shared_ptr<Label> frameRateLabel;
    std::shared_ptr<Label> timeLabel;
    std::shared_ptr<Label> mousePositionLabel;
    std::shared_ptr<HorizontalControlList> layout;

    BottomPanel(App& app);
    void Update(App& app) override;
};
