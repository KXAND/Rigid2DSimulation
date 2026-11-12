#pragma once

#include <string>

#include "ControlContainer.h"
#include "FloatPanel.h"

class Connection;

class ConnectionPanel : public LocalizedFloatPanel
{
public:
    ConnectionPanel(App& app);
    void Discard(App& app) override;

private:
    VerticalStack* editorLayout = nullptr;

    // 仅用于识别选择切换并清理旧编辑状态，不拥有场景对象。
    Connection* selectedConnection = nullptr;

    void BuildEditor();
    void BuildActionButtons();
    void SyncSelection(App& app);
    void UpdateContent(App& app) override;
    static std::wstring GetDetailsText(App& app);
};
