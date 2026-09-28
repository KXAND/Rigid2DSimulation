#pragma once
#include "Label.h"
#include "Button.h"
#include "TextBox.h"
#include "ControlList.h"
#include "FloatPanel.h"
#include "RichLabel.h"
#include "Separator.h"

struct SaveButton : LocalizedButton
{
    SaveButton();
    void OnClick(App& app) override;
};
struct LoadButton : LocalizedButton
{
    LoadButton();
    void OnClick(App& app) override;
};
struct GlobalDetailsLabel : RichLabel
{
    void Sync(App& app) override;
};

struct GlobalPanel : LocalizedFloatPanel
{
    std::shared_ptr<TextBox> savePathTextBox;
    std::shared_ptr<SaveButton> saveButton;
    std::shared_ptr<HorizontalControlList> saveControls;
    std::shared_ptr<TextBox> loadPathTextBox;
    std::shared_ptr<LoadButton> loadButton;
    std::shared_ptr<HorizontalControlList> loadControls;
    std::shared_ptr<HorizontalSeparator> separator;
    std::shared_ptr<GlobalDetailsLabel> detailsLabel;
    std::shared_ptr<VerticalControlList> controls;
    GlobalPanel(App& app);
};
