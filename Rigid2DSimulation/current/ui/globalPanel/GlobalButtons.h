#pragma once

#include "Button.h"

struct SceneSaveButton : LocalizedButton
{
    SceneSaveButton();
    void OnClick(App& app) override;
};

struct SceneLoadButton : LocalizedButton
{
    SceneLoadButton();
    void OnClick(App& app) override;
};
