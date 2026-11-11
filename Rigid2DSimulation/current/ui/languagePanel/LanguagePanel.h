#pragma once

#include <memory>

#include "ControlList.h"
#include "FloatPanel.h"

class LanguagePanel : public LocalizedFloatPanel
{
public:
    LanguagePanel(App& app);

private:
    std::unique_ptr<VerticalControlList> layout;
};
