#pragma once

#include "Button.h"

struct ParameterDeletionButton : LocalizedButton
{
    int id = 0;
    ParameterDeletionButton(int id);
    void OnClick(App& app) override;
};

struct ParameterAdditionButton : LocalizedButton
{
    ParameterAdditionButton();
    void OnClick(App& app) override;
};
