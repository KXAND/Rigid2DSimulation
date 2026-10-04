#pragma once

#include "Button.h"

struct BodyScriptApplicationButton : LocalizedButton
{
    BodyScriptApplicationButton();
    void OnClick(App& app) override;
};

struct BodyDeletionButton : LocalizedButton
{
    BodyDeletionButton();
    void OnClick(App& app) override;
};
