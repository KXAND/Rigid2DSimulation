#include "ConnectionControls.h"

#include <memory>
#include <vector>

#include "Connection.h"
#include "SimulationApp.h"

ConnectionScriptEditor::ConnectionScriptEditor() : RichTextBox(360, 400) {}

void ConnectionScriptEditor::Sync(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    value = simulationApp.editor.connectionSelecting->temporaryScriptText;
}

void ConnectionScriptEditor::Upload(App& app) const
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    simulationApp.editor.connectionSelecting->temporaryScriptText = value;
}

ConnectionEditorControls::ConnectionEditorControls()
{
    vector<Control*> controls;
    scriptEditor = std::make_shared<ConnectionScriptEditor>();
    applyButton = std::make_shared<ConnectionScriptApplicationButton>();
    deleteButton = std::make_shared<ConnectionDeletionButton>();
    controls = {&*applyButton, &*deleteButton};
    actionButtons = std::make_shared<HorizontalControlList>(controls);
    controls = {};
    separator = std::make_shared<HorizontalSeparator>(400);
}

void ConnectionEditorControls::Update(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    auto* appSelectedConnection = simulationApp.editor.connectionSelecting;
    if (selectedConnection != appSelectedConnection)
    {
        selectedConnection = appSelectedConnection;
        for (auto control : controls)
        {
            control->Discard(app);
        }
        if (selectedConnection)
        {
            controls = {&*scriptEditor, &*actionButtons, &*separator};
        }
        else
        {
            controls = {};
        }
    }
    VerticalControlList::Update(app);
}

void ConnectionEditorControls::Discard(App& app)
{
    VerticalControlList::Discard(app);
    selectedConnection = NULL;
    controls = {};
}

void ConnectionDetailsLabel::Sync(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    auto* selectedConnection = simulationApp.editor.connectionSelecting;
    if (!selectedConnection)
    {
        text = simulationApp.GetLocalizedString(L"noConnectionSelected");
        return;
    }
    auto& connection = *selectedConnection;
    text.clear();

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
}
