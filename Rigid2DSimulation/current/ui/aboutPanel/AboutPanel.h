#pragma once

#include <memory>

#include "ControlList.h"
#include "FloatPanel.h"
#include "RichLabel.h"

class AboutPanel : public LocalizedFloatPanel
{
public:
    std::shared_ptr<LocalizedRichLabel> aboutText;
    std::shared_ptr<VerticalControlList> controls;
    AboutPanel(App& app);
};
