#pragma once

#include "Button.h"

class SceneSaveButton : public LocalizedButton
{
public:
    SceneSaveButton();
    void OnClick(App& app) override;
};

class SceneLoadButton : public LocalizedButton
{
public:
    SceneLoadButton();
    void OnClick(App& app) override;
};
