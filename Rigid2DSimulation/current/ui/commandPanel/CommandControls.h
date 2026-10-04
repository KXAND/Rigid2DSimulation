#pragma once

#include "RichTextBox.h"

struct CommandScriptEditor : RichTextBox
{
    CommandScriptEditor();
    void Sync(App& app) override;
    void OnDone(App& app) const override;
};
