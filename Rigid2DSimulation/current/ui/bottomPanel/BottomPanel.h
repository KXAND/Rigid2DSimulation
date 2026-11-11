#pragma once

#include <memory>

#include "ControlList.h"
#include "Label.h"
#include "Panel.h"

class BottomPanel : public Panel
{
public:
    BottomPanel(App& app);
    void Update(App& app) override;

private:
    std::unique_ptr<HorizontalControlList> layout;
    Label* frameRateLabel = nullptr;
    Label* timeLabel = nullptr;
    Label* mousePositionLabel = nullptr;
};
