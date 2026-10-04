#pragma once

#include <memory>

#include "ControlList.h"
#include "FloatPanel.h"
#include "RichLabel.h"

struct AboutPanel : LocalizedFloatPanel
{
    std::shared_ptr<LocalizedRichLabel> aboutText;
    std::shared_ptr<VerticalControlList> controls;
    AboutPanel(App& app);
};
