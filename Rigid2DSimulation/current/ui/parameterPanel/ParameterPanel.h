#pragma once

// 脚本参数面板，负责编辑传入脚本作用域的命名参数。
// 当前暂停脚本接入：UI 不创建该面板，源文件不参与构建。
#include "ControlContainer.h"
#include "FloatPanel.h"

class ParameterPanel : public LocalizedFloatPanel
{
public:
    ParameterPanel(App& app);

private:
    VerticalStack* parameterList = nullptr;

    void CreateParameterRow(int id);
    void SyncParameterRows(App& app);
    void UpdateContent(App& app) override;
};
