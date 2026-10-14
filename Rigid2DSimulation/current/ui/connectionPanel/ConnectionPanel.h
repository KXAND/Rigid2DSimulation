#pragma once

#include <memory>
#include <string>

#include "Button.h"
#include "ControlList.h"
#include "FloatPanel.h"
#include "RichLabel.h"
#include "RichTextBox.h"
#include "Separator.h"

class Connection;

class ConnectionPanel : public LocalizedFloatPanel
{
public:
    std::shared_ptr<RichTextBox> scriptEditor;
    std::shared_ptr<LocalizedButton> applyButton;
    std::shared_ptr<LocalizedButton> deleteButton;
    std::shared_ptr<HorizontalControlList> actionButtons;
    std::shared_ptr<HorizontalSeparator> separator;
    std::shared_ptr<VerticalControlList> editorLayout;
    std::shared_ptr<RichLabel> detailsLabel;
    std::shared_ptr<VerticalControlList> layout;

    ConnectionPanel(App& app);
    void Discard(App& app) override;

private:
    // 仅用于识别选择切换并清理旧编辑状态，不拥有场景对象。
    Connection* selectedConnection = nullptr;

    void BuildEditor();
    void BuildActionButtons();
    void SyncSelection(App& app);
    void UpdateContent(App& app) override;
    static std::wstring GetDetailsText(App& app);
};
