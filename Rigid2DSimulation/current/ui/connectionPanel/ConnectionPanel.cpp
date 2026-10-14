#include "ConnectionPanel.h"

#include <memory>
#include <vector>

#include "Connection.h"
#include "SimulationApp.h"

ConnectionPanel::ConnectionPanel(App& app) : LocalizedFloatPanel(app, 400, 600, L"connectionPanelTitle")
{
    BuildEditor();

    detailsLabel = std::make_shared<RichLabel>();
    detailsLabel->textProvider = GetDetailsText;

    layout = std::make_shared<VerticalControlList>(
        std::vector<Control*>{editorLayout.get(), detailsLabel.get()});
    contentControl = layout.get();
    Init(app);
}

void ConnectionPanel::BuildEditor()
{
    scriptEditor = std::make_shared<RichTextBox>(360, 400);
    scriptEditor->valueProvider = [](App& app)
    {
        return static_cast<SimulationApp&>(app).editor.connectionSelecting->temporaryScriptText;
    };
    scriptEditor->onEditing = [](App& app, std::wstring const& text)
    {
        static_cast<SimulationApp&>(app).editor.connectionSelecting->temporaryScriptText = text;
    };

    BuildActionButtons();
    separator = std::make_shared<HorizontalSeparator>(400);
    editorLayout = std::make_shared<VerticalControlList>();
}

void ConnectionPanel::BuildActionButtons()
{
    applyButton = std::make_shared<LocalizedButton>(70, L"applyConnectionButton");
    applyButton->onClick = [](App& app)
    {
        auto& simulationApp = static_cast<SimulationApp&>(app);
        auto& selectedConnection = *simulationApp.editor.connectionSelecting;
        selectedConnection.scriptText = selectedConnection.temporaryScriptText;
        simulationApp.scriptRuntime.RefreshConnectionScript(selectedConnection);
    };
    deleteButton = std::make_shared<LocalizedButton>(70, L"deleteConnectionButton");
    deleteButton->onClick = [](App& app)
    {
        auto& simulationApp = static_cast<SimulationApp&>(app);
        simulationApp.editor.RequestConnectionDeletion(simulationApp.physicsWorld);
    };
    actionButtons = std::make_shared<HorizontalControlList>(
        std::vector<Control*>{applyButton.get(), deleteButton.get()});
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
    if (selectedConnection)
    {
        editorLayout->controls = {scriptEditor.get(), actionButtons.get(), separator.get()};
    }
    else
    {
        editorLayout->controls.clear();
    }
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
    editorLayout->controls.clear();
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
