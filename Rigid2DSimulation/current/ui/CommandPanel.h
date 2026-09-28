#pragma once
#include "Button.h"
#include "ControlList.h"
#include "FloatPanel.h"
#include "RichTextBox.h"

struct CommandScriptEditor : RichTextBox
{
    CommandScriptEditor();
    void Sync(App& app) override;
    void OnDone(App& app) const override;
};
struct CompileButton : LocalizedButton
{
    CompileButton();
    void OnClick(App& app) override;
};

struct CommandPanel : LocalizedFloatPanel
{
    std::shared_ptr<CommandScriptEditor> scriptEditor;
    std::shared_ptr<CompileButton> compileButton;
    std::shared_ptr<VerticalControlList> controls;
    CommandPanel(App& app);
};
