#pragma once

#include <memory>

#include "FloatPanel.h"
#include "RichLabel.h"

class AboutPanel : public LocalizedFloatPanel
{
public:
    std::shared_ptr<LocalizedRichLabel> aboutText;
    AboutPanel(App& app);
};
