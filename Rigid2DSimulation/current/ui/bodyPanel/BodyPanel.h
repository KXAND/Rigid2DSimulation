#pragma once

#include <memory>
#include <string>

#include "ControlList.h"
#include "FloatPanel.h"

class Body;

class BodyPanel : public LocalizedFloatPanel
{
public:
    BodyPanel(App& app);
    void Discard(App& app) override;

private:
    std::unique_ptr<VerticalControlList> layout;
    VerticalControlList* editorLayout = nullptr;
    VerticalControlList* connectionList = nullptr;

    // 仅用于识别选择切换并清理旧编辑状态，不拥有场景对象。
    Body* selectedBody = nullptr;

    void BuildEditor();
    void BuildActionButtons();
    void SyncSelection(App& app);
    void CreateConnectionRow(int id);
    static std::wstring GetConnectionText(App& app, int id);
    void SyncConnectionRows(App& app);
    void UpdateContent(App& app) override;
    static std::wstring GetDetailsText(App& app);
};
