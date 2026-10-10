#include "CreationButtons.h"

#include "SimulationApp.h"

CreationToolButton::CreationToolButton(int toolMode, std::wstring const& textId)
    : LocalizedButton(340, textId), toolMode(toolMode)
{
}

void CreationToolButton::OnClick(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    simulationApp.editor.SelectCreationTool(toolMode);
}
