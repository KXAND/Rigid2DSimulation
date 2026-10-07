#pragma once

#include "RichTextBox.h"

class CommandScriptEditor : public RichTextBox
{
public:
    CommandScriptEditor();
    void Sync(App& app) override;
    void OnDone(App& app) const override;
};
