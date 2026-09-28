#pragma once
#include "Button.h"
#include "ControlList.h"
#include "FloatPanel.h"
#include "RichTextBox.h"
#include "RichLabel.h"
#include "Separator.h"

struct Connection;
struct ConnectionScriptEditor : RichTextBox
{
    ConnectionScriptEditor();
    void Sync(App& app) override;
    void Upload(App& app) const override;
};
struct ApplyConnectionButton : LocalizedButton
{
    ApplyConnectionButton();
    void OnClick(App& app) override;
};
struct DeleteConnectionButton : LocalizedButton
{
    DeleteConnectionButton();
    void OnClick(App& app) override;
};

struct ConnectionEditorControls : VerticalControlList
{
    Connection* selectedConnection = NULL;
    std::shared_ptr<ConnectionScriptEditor> scriptEditor;
    std::shared_ptr<ApplyConnectionButton> applyButton;
    std::shared_ptr<DeleteConnectionButton> deleteButton;
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

struct ConnectionPanel : LocalizedFloatPanel
{
    std::shared_ptr<ConnectionEditorControls> editorControls;
    std::shared_ptr<ConnectionDetailsLabel> detailsLabel;
    std::shared_ptr<VerticalControlList> controls;
    ConnectionPanel(App& app);
};
