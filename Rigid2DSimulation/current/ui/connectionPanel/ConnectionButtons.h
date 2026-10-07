#pragma once

#include "Button.h"

class ConnectionScriptApplicationButton : public LocalizedButton
{
public:
    ConnectionScriptApplicationButton();
    void OnClick(App& app) override;
};

class ConnectionDeletionButton : public LocalizedButton
{
public:
    ConnectionDeletionButton();
    void OnClick(App& app) override;
};
