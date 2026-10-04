#pragma once

#include "Button.h"

struct ChineseLanguageButton : Button
{
    ChineseLanguageButton();
    void OnClick(App& app) override;
};

struct EnglishLanguageButton : Button
{
    EnglishLanguageButton();
    void OnClick(App& app) override;
};
