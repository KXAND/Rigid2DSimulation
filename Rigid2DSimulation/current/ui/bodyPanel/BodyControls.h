#pragma once

#include <memory>
#include <vector>

#include "Button.h"
#include "CheckBox.h"
#include "ControlList.h"
#include "Label.h"
#include "RichLabel.h"
#include "RichTextBox.h"
#include "Separator.h"

class Body;

class BodyScriptEditor : public RichTextBox
{
public:
    BodyScriptEditor();
    void Sync(App& app) override;
    void Upload(App& app) const override;
};

class BodyConnectionLabel : public Label
{
public:
    int id = 0;
    BodyConnectionLabel(int id);
    void Sync(App& app) override;
};

class SelectBodyConnectionCheckBox : public LocalizedCheckBox
{
public:
    int id = 0;
    SelectBodyConnectionCheckBox(int id);
    void Sync(App& app) override;
    void Upload(App& app) const override;
};

class BodyConnectionControlRow : public HorizontalControlList
{
public:
    std::shared_ptr<BodyConnectionLabel> connectionLabel;
    std::shared_ptr<SelectBodyConnectionCheckBox> selectCheckBox;
    BodyConnectionControlRow(int id);
};

class BodyConnectionControlList : public VerticalControlList
{
public:
    vector<std::shared_ptr<BodyConnectionControlRow>> connectionRows;
    void Update(App& app) override;
};

class BodyDetailsLabel : public RichLabel
{
public:
    void Sync(App& app) override;
};

// 我觉得 delegate 没有啥价值了。
// 其实 Panel 的 set_c 方法也没啥价值了吧。
class BodyEditorControls : public VerticalControlList
{
public:
    Body* selectedBody = NULL;
    std::shared_ptr<BodyConnectionControlList> connectionList;
    std::shared_ptr<BodyScriptEditor> scriptEditor;
    std::shared_ptr<LocalizedButton> applyButton;
    std::shared_ptr<LocalizedButton> deleteButton;
    std::shared_ptr<HorizontalControlList> actionButtons;
    std::shared_ptr<HorizontalSeparator> separator;
    BodyEditorControls();
    void Update(App& app) override;
    void Discard(App& app) override;
};
