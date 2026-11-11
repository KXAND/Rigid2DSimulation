#pragma once

#include <memory>
#include <string>

#include "ControlList.h"
#include "FloatPanel.h"

class TextBox;

class GlobalPanel : public LocalizedFloatPanel
{
public:
    // 控件由 layout 持有；这里只借用保存、加载操作需要读取的输入框。
    TextBox* savePathTextBox = nullptr;
    TextBox* loadPathTextBox = nullptr;

    GlobalPanel(App& app);

private:
    std::unique_ptr<VerticalControlList> layout;

    static void SaveScene(App& app);
    static void LoadScene(App& app);
    static std::wstring GetDetailsText(App& app);
};
