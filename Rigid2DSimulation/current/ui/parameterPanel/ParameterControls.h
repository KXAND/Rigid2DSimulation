#pragma once

#include <memory>
#include <vector>

#include "ControlList.h"
#include "DragBar.h"
#include "Button.h"
#include "Separator.h"
#include "TextBox.h"

class ParameterNameTextBox : public TextBox
{
public:
    int id = 0;
    ParameterNameTextBox(int id);
    void Sync(App& app) override;
    void OnDone(App& app) const override;
};

class ParameterValueDragBar : public HorizontalDragBar
{
public:
    int id = 0;
    ParameterValueDragBar(int id);
    void Sync(App& app) override;
    void Upload(App& app) const override;
};

class ParameterValueTextBox : public TextBox
{
public:
    int id = 0;
    ParameterValueTextBox(int id);
    void Sync(App& app) override;
    void OnDone(App& app) const override;
};

class ParameterControlRow : public VerticalControlList
{
public:
    std::shared_ptr<ParameterNameTextBox> nameTextBox;
    std::shared_ptr<LocalizedButton> deleteButton;
    std::shared_ptr<HorizontalControlList> nameControls;
    std::shared_ptr<ParameterValueDragBar> valueDragBar;
    std::shared_ptr<ParameterValueTextBox> valueTextBox;
    std::shared_ptr<HorizontalControlList> valueControls;
    std::shared_ptr<HorizontalSeparator> separator;
    ParameterControlRow(int id);
};

class ParameterControlList : public VerticalControlList
{
public:
    vector<std::shared_ptr<ParameterControlRow>> parameterRows;
    void Update(App& app) override;
};
