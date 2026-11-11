#pragma once

#include <memory>

#include "ControlList.h"
#include "FloatPanel.h"

class AboutPanel : public LocalizedFloatPanel
{
public:
    AboutPanel(App& app);

private:
    std::unique_ptr<VerticalControlList> layout;
};
