#include "LanguagePanel.h"

#include <memory>
#include <vector>

LanguagePanel::LanguagePanel(App& app) : LocalizedFloatPanel(app, 200, 200, L"languagePanelTitle")
{
    vector<Control*> controls;
    shown = true;
    chineseButton = std::make_shared<ChineseLanguageButton>();
    englishButton = std::make_shared<EnglishLanguageButton>();
    controls = {&*chineseButton, &*englishButton};
    this->controls = std::make_shared<VerticalControlList>(controls);
    contentControl = &*this->controls;
    Init(app);
}
