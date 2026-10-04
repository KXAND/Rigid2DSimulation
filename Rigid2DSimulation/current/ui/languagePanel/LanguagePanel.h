#pragma once

#include <memory>

#include "ControlList.h"
#include "FloatPanel.h"
#include "LanguageButtons.h"

struct LanguagePanel : LocalizedFloatPanel
{
    std::shared_ptr<ChineseLanguageButton> chineseButton;
    std::shared_ptr<EnglishLanguageButton> englishButton;
    std::shared_ptr<VerticalControlList> controls;
    LanguagePanel(App& app);
};
