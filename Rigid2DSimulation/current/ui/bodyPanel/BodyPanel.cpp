#include "BodyPanel.h"

#include <memory>
#include <vector>

#include "Body.h"
#include "Connection.h"
#include "SimulationApp.h"

BodyPanel::BodyPanel(App& app) : LocalizedFloatPanel(app, 400, 600, L"bodyPanelTitle")
{
    BuildEditor();

    detailsLabel = std::make_shared<RichLabel>();
    detailsLabel->textProvider = GetDetailsText;

    layout = std::make_shared<VerticalControlList>(
        std::vector<Control*>{editorLayout.get(), detailsLabel.get()});
    contentControl = layout.get();
    Init(app);
}

void BodyPanel::BuildEditor()
{
    connectionList = std::make_shared<VerticalControlList>();
    scriptEditor = std::make_shared<RichTextBox>(360, 400);
    scriptEditor->valueProvider = [](App& app)
    {
        return static_cast<SimulationApp&>(app).editor.bodySelecting->scriptData.temporaryScriptText;
    };
    scriptEditor->onEditing = [](App& app, std::wstring const& text)
    {
        static_cast<SimulationApp&>(app).editor.bodySelecting->scriptData.temporaryScriptText = text;
    };

    BuildActionButtons();
    separator = std::make_shared<HorizontalSeparator>(400);
    editorLayout = std::make_shared<VerticalControlList>();
}

void BodyPanel::BuildActionButtons()
{
    applyButton = std::make_shared<LocalizedButton>(70, L"applyBodyButton");
    applyButton->onClick = [](App& app)
    {
        auto& simulationApp = static_cast<SimulationApp&>(app);
        auto& selectedBody = *simulationApp.editor.bodySelecting;
        selectedBody.scriptData.scriptText = selectedBody.scriptData.temporaryScriptText;
        simulationApp.scriptRuntime.RefreshBodyScript(selectedBody);
    };
    deleteButton = std::make_shared<LocalizedButton>(70, L"deleteBodyButton");
    deleteButton->onClick = [](App& app)
    {
        auto& simulationApp = static_cast<SimulationApp&>(app);
        simulationApp.editor.RequestBodyDeletion(simulationApp.physicsWorld);
    };
    actionButtons = std::make_shared<HorizontalControlList>(
        std::vector<Control*>{applyButton.get(), deleteButton.get()});
}

void BodyPanel::SyncSelection(App& app)
{
    auto* selection = static_cast<SimulationApp&>(app).editor.bodySelecting;
    if (selectedBody == selection)
    {
        return;
    }

    selectedBody = selection;
    editorLayout->Discard(app);
    if (selectedBody)
    {
        editorLayout->controls = {connectionList.get(), scriptEditor.get(), actionButtons.get(), separator.get()};
    }
    else
    {
        editorLayout->controls.clear();
    }
}

void BodyPanel::UpdateContent(App& app)
{
    SyncSelection(app);
    if (selectedBody)
    {
        SyncConnectionRows(app);
    }
    FloatPanel::UpdateContent(app);
}

void BodyPanel::Discard(App& app)
{
    FloatPanel::Discard(app);
    selectedBody = nullptr;
    editorLayout->controls.clear();
}

BodyPanel::ConnectionRow BodyPanel::CreateConnectionRow(int id)
{
    ConnectionRow row;
    row.connectionLabel = std::make_shared<Label>();
    row.connectionLabel->width = 270;
    row.connectionLabel->fixedWidth = true;
    row.connectionLabel->textProvider = [id](App& app)
    {
        return GetConnectionText(app, id);
    };

    row.selectCheckBox = std::make_shared<LocalizedCheckBox>(L"selectBodyConnection");
    row.selectCheckBox->checkedProvider = [id](App& app)
    {
        auto& editor = static_cast<SimulationApp&>(app).editor;
        return editor.connectionSelecting == editor.bodySelecting->GetConnections()[id];
    };
    row.selectCheckBox->onCheckedChanged = [id](App& app, bool checked)
    {
        auto& editor = static_cast<SimulationApp&>(app).editor;
        editor.connectionSelecting = checked ? editor.bodySelecting->GetConnections()[id] : nullptr;
    };
    row.layout = std::make_shared<HorizontalControlList>(
        std::vector<Control*>{row.connectionLabel.get(), row.selectCheckBox.get()});
    return row;
}

void BodyPanel::SyncConnectionRows(App& app)
{
    int size = static_cast<int>(selectedBody->GetConnections().size());
    for (int id = static_cast<int>(connectionRows.size()); id < size; ++id)
    {
        connectionRows.push_back(CreateConnectionRow(id));
    }
    connectionList->controls.clear();
    for (int id = 0; id < size; ++id)
    {
        connectionList->controls.push_back(connectionRows[id].layout.get());
    }
    for (int id = size; id < static_cast<int>(connectionRows.size()); ++id)
    {
        connectionRows[id].layout->Discard(app);
    }
}

std::wstring BodyPanel::GetConnectionText(App& app, int id)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    auto* connection = simulationApp.editor.bodySelecting->GetConnections()[id];
    wstring type;
    switch (connection->type)
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
    return simulationApp.GetLocalizedString(L"connectionLabel") + std::to_wstring(id) + L": " + type;
}

std::wstring BodyPanel::GetDetailsText(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    auto* selectedBody = simulationApp.editor.bodySelecting;
    if (!selectedBody)
    {
        return simulationApp.GetLocalizedString(L"noBodySelected");
    }
    auto& body = *selectedBody;
    std::wstring text;

    text += simulationApp.GetLocalizedString(L"bodyPosition") + ToWString(body.position) + L"\n";
    text += simulationApp.GetLocalizedString(L"bodyVelocity") + ToWString(body.velocity) + L"\n";
    text += simulationApp.GetLocalizedString(L"bodyAngle") + FormatFixed2(body.radian) + L"\n";
    text += simulationApp.GetLocalizedString(L"bodyAngularVelocity") + FormatFixed2(body.velocityAngular) + L"\n";
    text += simulationApp.GetLocalizedString(L"bodyFixed") + std::to_wstring(body.fixed) + L"\n";
    text += simulationApp.GetLocalizedString(L"bodyDensity") + FormatFixed2(body.density) + L"\n";
    text += simulationApp.GetLocalizedString(L"bodyChargeDensity") + FormatFixed2(body.chargeDensity) + L"\n";
    text += simulationApp.GetLocalizedString(L"bodyElasticity") + FormatFixed2(body.elasticity) + L"\n";
    text += simulationApp.GetLocalizedString(L"bodyStaticFriction") + FormatFixed2(body.frictionStatic) + L"\n";
    text += simulationApp.GetLocalizedString(L"bodyDynamicFriction") + FormatFixed2(body.frictionDynamic) + L"\n";
    text += simulationApp.GetLocalizedString(L"bodyVelocityDamping") + FormatFixed2(body.dampCoeff) + L"\n";
    text += simulationApp.GetLocalizedString(L"bodyAngularVelocityDamping") + FormatFixed2(body.dampCoeffAngular) + L"\n";
    return text;
}
