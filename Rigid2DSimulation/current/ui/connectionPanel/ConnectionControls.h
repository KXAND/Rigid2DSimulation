#pragma once

#include <memory>

#include "ConnectionButtons.h"
#include "ControlList.h"
#include "RichLabel.h"
#include "RichTextBox.h"
#include "Separator.h"

struct Connection;

struct ConnectionScriptEditor : RichTextBox
{
    ConnectionScriptEditor();
    void Sync(App& app) override;
    void Upload(App& app) const override;
};

struct ConnectionEditorControls : VerticalControlList
{
    Connection* selectedConnection = NULL;
    std::shared_ptr<ConnectionScriptEditor> scriptEditor;
    std::shared_ptr<ConnectionScriptApplicationButton> applyButton;
    std::shared_ptr<ConnectionDeletionButton> deleteButton;
    std::shared_ptr<HorizontalControlList> actionButtons;
    std::shared_ptr<HorizontalSeparator> separator;
    ConnectionEditorControls();
    void Update(App& app) override;
    void Discard(App& app) override;
};

struct ConnectionDetailsLabel : RichLabel
{
    void Sync(App& app) override;
};
