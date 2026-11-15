#include "ParameterPanel.h"

// 保留脚本参数面板实现；恢复运行时接入前不编译、不实例化。
#include <memory>
#include <utility>

#include "DragBar.h"
#include "SceneParameter.h"
#include "SimulationApp.h"
#include "TextBox.h"

ParameterPanel::ParameterPanel(App& app) : LocalizedFloatPanel(app.screen.Rect(), 400, 600, L"parameterPanelTitle")
{
    auto layout = std::make_unique<VerticalStack>();
    parameterList = &layout->AddColumn();

    layout->AddLocalizedButton(160, L"addParameterButton", [](App& app)
    {
        auto& simulationApp = static_cast<SimulationApp&>(app);
        simulationApp.scriptRuntime.parameters.push_back(
            std::make_shared<SceneParameter>(simulationApp.GetLocalizedString(L"unnamedParameter")));
    });

    SetContent(std::move(layout));
}

void ParameterPanel::CreateParameterRow(int id)
{
    auto& row = parameterList->AddColumn();
    auto& nameRow = row.AddRow();
    auto& nameTextBox = nameRow.AddTextBox(270);
    nameTextBox.valueProvider = [id](App& app)
    {
        return static_cast<SimulationApp&>(app).scriptRuntime.parameters[id]->name;
    };
    nameTextBox.onEditFinished = [id](App& app, std::wstring const& text)
    {
        static_cast<SimulationApp&>(app).scriptRuntime.parameters[id]->name = text;
    };
    nameRow.AddLocalizedButton(80, L"deleteParameterButton", [id](App& app)
    {
        static_cast<SimulationApp&>(app).scriptRuntime.parameters[id]->isMarkedForDeletion = true;
    });

    auto& valueRow = row.AddRow();
    auto& valueDragBar = valueRow.AddDragBar(270);
    valueDragBar.valueProvider = [id](App& app)
    {
        return static_cast<SimulationApp&>(app).scriptRuntime.parameters[id]->value;
    };
    valueDragBar.onDragging = [id](App& app, double value)
    {
        static_cast<SimulationApp&>(app).scriptRuntime.parameters[id]->value = value;
    };
    auto& valueTextBox = valueRow.AddTextBox(80);
    valueTextBox.valueProvider = [id](App& app)
    {
        return FormatFixed2(static_cast<SimulationApp&>(app).scriptRuntime.parameters[id]->value);
    };
    valueTextBox.onEditFinished = [id](App& app, std::wstring const& text)
    {
        static_cast<SimulationApp&>(app).scriptRuntime.parameters[id]->value = ParseDouble(text);
    };

    row.AddSeparator(400);
}

void ParameterPanel::SyncParameterRows(App& app)
{
    auto const& parameters = static_cast<SimulationApp&>(app).scriptRuntime.parameters;
    auto size = parameters.size();
    for (auto id = parameterList->GetSize(); id < size; ++id)
    {
        CreateParameterRow(static_cast<int>(id));
    }
    parameterList->SetVisibleCount(size, app);
}

void ParameterPanel::UpdateContent(App& app)
{
    SyncParameterRows(app);
    FloatPanel::UpdateContent(app);
}
