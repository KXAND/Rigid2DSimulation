#pragma once

#include "Button.h"

class ParameterDeletionButton : public LocalizedButton
{
public:
    int id = 0;
    ParameterDeletionButton(int id);
    void OnClick(App& app) override;
};

class ParameterAdditionButton : public LocalizedButton
{
public:
    ParameterAdditionButton();
    void OnClick(App& app) override;
};
