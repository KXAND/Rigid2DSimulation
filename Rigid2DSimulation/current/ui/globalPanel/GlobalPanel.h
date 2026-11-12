#pragma once

#include <string>

#include "ControlContainer.h"
#include "FloatPanel.h"

class TextBox;

class GlobalPanel : public LocalizedFloatPanel
{
public:
    // 内容由基类持有；这里只借用保存、加载操作需要读取的输入框。
    TextBox* savePathTextBox = nullptr;
    TextBox* loadPathTextBox = nullptr;

    GlobalPanel(App& app);

private:
    static void SaveScene(App& app);
    static void LoadScene(App& app);
    static std::wstring GetDetailsText(App& app);
};
