#pragma once

#include "Button.h"

struct ConnectionScriptApplicationButton : LocalizedButton
{
    ConnectionScriptApplicationButton();
    void OnClick(App& app) override;
};

struct ConnectionDeletionButton : LocalizedButton
{
    ConnectionDeletionButton();
    void OnClick(App& app) override;
};
