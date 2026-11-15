#pragma once

// 脚本命令面板，负责编辑和执行场景脚本，并非通用命令面板。
// 当前暂停脚本接入：UI 不创建该面板，源文件不参与构建。
#include "ControlContainer.h"
#include "FloatPanel.h"

class CommandPanel : public LocalizedFloatPanel
{
public:
    CommandPanel(App& app);

};
