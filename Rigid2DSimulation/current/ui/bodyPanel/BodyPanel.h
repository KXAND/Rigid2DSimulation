#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Button.h"
#include "CheckBox.h"
#include "ControlList.h"
#include "FloatPanel.h"
#include "Label.h"
#include "RichLabel.h"
#include "RichTextBox.h"
#include "Separator.h"

class Body;

class BodyPanel : public LocalizedFloatPanel
{
public:
    struct ConnectionRow
    {
        std::shared_ptr<Label> connectionLabel;
        std::shared_ptr<LocalizedCheckBox> selectCheckBox;
        std::shared_ptr<HorizontalControlList> layout;
    };

    std::vector<ConnectionRow> connectionRows;
    std::shared_ptr<VerticalControlList> connectionList;
    std::shared_ptr<RichTextBox> scriptEditor;
    std::shared_ptr<LocalizedButton> applyButton;
    std::shared_ptr<LocalizedButton> deleteButton;
    std::shared_ptr<HorizontalControlList> actionButtons;
    std::shared_ptr<HorizontalSeparator> separator;
    std::shared_ptr<VerticalControlList> editorLayout;
    std::shared_ptr<RichLabel> detailsLabel;
    std::shared_ptr<VerticalControlList> layout;

    BodyPanel(App& app);
    void Discard(App& app) override;

private:
    // 仅用于识别选择切换并清理旧编辑状态，不拥有场景对象。
    Body* selectedBody = nullptr;

    void BuildEditor();
    void BuildActionButtons();
    void SyncSelection(App& app);
    static ConnectionRow CreateConnectionRow(int id);
    static std::wstring GetConnectionText(App& app, int id);
    void SyncConnectionRows(App& app);
    void UpdateContent(App& app) override;
    static std::wstring GetDetailsText(App& app);
};
