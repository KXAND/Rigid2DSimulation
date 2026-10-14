#pragma once

#include <memory>

#include "Button.h"
#include "ControlList.h"
#include "RichLabel.h"
#include "RichTextBox.h"
#include "Separator.h"

class Connection;

class ConnectionScriptEditor : public RichTextBox
{
public:
    ConnectionScriptEditor();
    void Sync(App& app) override;
    void Upload(App& app) const override;
};

class ConnectionEditorControls : public VerticalControlList
{
public:
    Connection* selectedConnection = NULL;
    std::shared_ptr<ConnectionScriptEditor> scriptEditor;
    std::shared_ptr<LocalizedButton> applyButton;
    std::shared_ptr<LocalizedButton> deleteButton;
    std::shared_ptr<HorizontalControlList> actionButtons;
    std::shared_ptr<HorizontalSeparator> separator;
    ConnectionEditorControls();
    void Update(App& app) override;
    void Discard(App& app) override;
};

class ConnectionDetailsLabel : public RichLabel
{
public:
    void Sync(App& app) override;
};
