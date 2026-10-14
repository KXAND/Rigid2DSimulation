#pragma once

#include <memory>

#include "ControlList.h"
#include "FloatPanel.h"
#include "Button.h"

class LanguagePanel : public LocalizedFloatPanel
{
public:
    std::shared_ptr<Button> chineseButton;
    std::shared_ptr<Button> englishButton;
    std::shared_ptr<VerticalControlList> layout;
    LanguagePanel(App& app);
};
