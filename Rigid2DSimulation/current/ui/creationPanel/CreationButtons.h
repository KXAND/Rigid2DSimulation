#pragma once

#include <string>

#include "Button.h"

class CreationToolButton : public LocalizedButton
{
public:
    CreationToolButton(int toolMode, std::wstring const& textId);
    void OnClick(App& app) override;

private:
    int toolMode;
};
