#pragma once

#include "Button.h"

class BodyScriptApplicationButton : public LocalizedButton
{
public:
    BodyScriptApplicationButton();
    void OnClick(App& app) override;
};

class BodyDeletionButton : public LocalizedButton
{
public:
    BodyDeletionButton();
    void OnClick(App& app) override;
};
