#include "ConnectionPanel.h"

#include <memory>
#include <utility>

#include "Connection.h"
#include "SimulationApp.h"

ConnectionPanel::ConnectionPanel(App& app) : LocalizedFloatPanel(app.screen.Rect(), 400, 600, L"connectionPanelTitle")
{
    auto layout = std::make_unique<VerticalStack>();
    editorLayout = &layout->AddColumn();
    BuildEditor();
    editorLayout->SetVisibleCount(0, app);

    layout->AddRichLabel(0, GetDetailsText);
    SetContent(std::move(layout));
}

void ConnectionPanel::BuildEditor()
{
    BuildActionButtons();
    editorLayout->AddSeparator(400);
}

void ConnectionPanel::BuildActionButtons()
{
    auto& actionButtons = editorLayout->AddRow();
    actionButtons.AddLocalizedButton(70, L"deleteConnectionButton", [](App& app)
    {
        auto& simulationApp = static_cast<SimulationApp&>(app);
        simulationApp.editor.RequestConnectionDeletion(simulationApp.physicsWorld);
    });
}

void ConnectionPanel::SyncSelection(App& app)
{
    auto* selection = static_cast<SimulationApp&>(app).editor.connectionSelecting;
    if (selectedConnection == selection)
    {
        return;
    }

    selectedConnection = selection;
    editorLayout->Discard(app);
    editorLayout->SetVisibleCount(selectedConnection ? editorLayout->GetSize() : 0, app);
}

void ConnectionPanel::UpdateContent(App& app)
{
    SyncSelection(app);
    FloatPanel::UpdateContent(app);
}

void ConnectionPanel::Discard(App& app)
{
    FloatPanel::Discard(app);
    selectedConnection = nullptr;
    editorLayout->SetVisibleCount(0, app);
}

std::wstring ConnectionPanel::GetDetailsText(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    auto* selectedConnection = simulationApp.editor.connectionSelecting;
    if (!selectedConnection)
    {
        return simulationApp.GetLocalizedString(L"noConnectionSelected");
    }
    auto& connection = *selectedConnection;
    std::wstring text;

    wstring type;
    switch (connection.type)
    {
    case kRopeConnection:
        type = simulationApp.GetLocalizedString(L"ropeConnection");
        break;
    case kLinkConnection:
        type = simulationApp.GetLocalizedString(L"linkConnection");
        break;
    case kSpringConnection:
        type = simulationApp.GetLocalizedString(L"springConnection");
        break;
    case kCordConnection:
        type = simulationApp.GetLocalizedString(L"cordConnection");
        break;
    }
    text += simulationApp.GetLocalizedString(L"connectionType") + type + L"\n";
    text += simulationApp.GetLocalizedString(L"connectionLength") + FormatFixed2(connection.length) + L"\n";
    text += simulationApp.GetLocalizedString(L"springConstant") + FormatFixed2(connection.springConstant) + L"\n";
    return text;
}
