#pragma once

#include "Button.h"

class ChineseLanguageButton : public Button
{
public:
    ChineseLanguageButton();
    void OnClick(App& app) override;
};

class EnglishLanguageButton : public Button
{
public:
    EnglishLanguageButton();
    void OnClick(App& app) override;
};
