#include "BottomPanel.h"

#include <memory>
#include <vector>

BottomPanel::BottomPanel(App& app) : Panel(Panel::CreateBottomPanel(app))
{
    vector<Control*> controls;
    frameRateLabel = std::make_shared<FrameRateLabel>();
    timeLabel = std::make_shared<TimeLabel>();
    mousePositionLabel = std::make_shared<MousePositionLabel>();
    controls = {&*frameRateLabel, &*timeLabel, &*mousePositionLabel};
    this->controls = std::make_shared<HorizontalControlList>(controls);
    contentControl = &*this->controls;
    Init(app);
}
