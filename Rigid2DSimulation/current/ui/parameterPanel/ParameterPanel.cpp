#include "ParameterPanel.h"

#include <memory>
#include <vector>

#include "SceneParameter.h"
#include "SimulationApp.h"

ParameterPanel::ParameterPanel(App& app) : LocalizedFloatPanel(app, 400, 600, L"parameterPanelTitle")
{
    parameterList = std::make_shared<VerticalControlList>();

    addButton = std::make_shared<LocalizedButton>(160, L"addParameterButton");
    addButton->onClick = [](App& app)
    {
        auto& simulationApp = static_cast<SimulationApp&>(app);
        simulationApp.scriptRuntime.parameters.push_back(
            std::make_shared<SceneParameter>(simulationApp.GetLocalizedString(L"unnamedParameter")));
    };

    layout = std::make_shared<VerticalControlList>(std::vector<Control*>{parameterList.get(), addButton.get()});
    contentControl = layout.get();
    Init(app);
}

ParameterPanel::ParameterRow ParameterPanel::CreateParameterRow(int id)
{
    ParameterRow row;
    row.nameTextBox = std::make_shared<TextBox>(270);
    row.nameTextBox->valueProvider = [id](App& app)
    {
        return static_cast<SimulationApp&>(app).scriptRuntime.parameters[id]->name;
    };
    row.nameTextBox->onEditFinished = [id](App& app, std::wstring const& text)
    {
        static_cast<SimulationApp&>(app).scriptRuntime.parameters[id]->name = text;
    };
    row.deleteButton = std::make_shared<LocalizedButton>(80, L"deleteParameterButton");
    row.deleteButton->onClick = [id](App& app)
    {
        static_cast<SimulationApp&>(app).scriptRuntime.parameters[id]->isMarkedForDeletion = true;
    };
    row.nameRow = std::make_shared<HorizontalControlList>(
        std::vector<Control*>{row.nameTextBox.get(), row.deleteButton.get()});

    row.valueDragBar = std::make_shared<HorizontalDragBar>(270);
    row.valueDragBar->valueProvider = [id](App& app)
    {
        return static_cast<SimulationApp&>(app).scriptRuntime.parameters[id]->value;
    };
    row.valueDragBar->onDragging = [id](App& app, double value)
    {
        static_cast<SimulationApp&>(app).scriptRuntime.parameters[id]->value = value;
    };
    row.valueTextBox = std::make_shared<TextBox>(80);
    row.valueTextBox->valueProvider = [id](App& app)
    {
        return FormatFixed2(static_cast<SimulationApp&>(app).scriptRuntime.parameters[id]->value);
    };
    row.valueTextBox->onEditFinished = [id](App& app, std::wstring const& text)
    {
        static_cast<SimulationApp&>(app).scriptRuntime.parameters[id]->value = ParseDouble(text);
    };
    row.valueRow = std::make_shared<HorizontalControlList>(
        std::vector<Control*>{row.valueDragBar.get(), row.valueTextBox.get()});

    row.separator = std::make_shared<HorizontalSeparator>(400);
    row.layout = std::make_shared<VerticalControlList>(
        std::vector<Control*>{row.nameRow.get(), row.valueRow.get(), row.separator.get()});
    return row;
}

void ParameterPanel::SyncParameterRows(App& app)
{
    auto const& parameters = static_cast<SimulationApp&>(app).scriptRuntime.parameters;
    int size = static_cast<int>(parameters.size());
    parameterList->controls.clear();
    for (int id = static_cast<int>(parameterRows.size()); id < size; ++id)
    {
        parameterRows.push_back(CreateParameterRow(id));
    }
    for (int id = 0; id < size; ++id)
    {
        parameterList->controls.push_back(parameterRows[id].layout.get());
    }
    for (int id = size; id < static_cast<int>(parameterRows.size()); ++id)
    {
        parameterRows[id].layout->Discard(app);
    }
}

void ParameterPanel::UpdateContent(App& app)
{
    SyncParameterRows(app);
    FloatPanel::UpdateContent(app);
}
