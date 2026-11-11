#pragma once

#include <memory>

#include "ControlList.h"
#include "FloatPanel.h"

class CommandPanel : public LocalizedFloatPanel
{
public:
    CommandPanel(App& app);

private:
    std::unique_ptr<VerticalControlList> layout;
};
