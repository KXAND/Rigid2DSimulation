#pragma once

#include "Button.h"

struct ScriptExecutionButton : LocalizedButton
{
    ScriptExecutionButton();
    void OnClick(App& app) override;
};
