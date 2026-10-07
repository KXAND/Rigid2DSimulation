#pragma once

#include "Button.h"

class ScriptExecutionButton : public LocalizedButton
{
public:
    ScriptExecutionButton();
    void OnClick(App& app) override;
};
