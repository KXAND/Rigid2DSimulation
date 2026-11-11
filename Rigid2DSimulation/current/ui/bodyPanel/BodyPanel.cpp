#include "BodyPanel.h"

#include <memory>

#include "Body.h"
#include "CheckBox.h"
#include "Connection.h"
#include "Label.h"
#include "RichTextBox.h"
#include "SimulationApp.h"

BodyPanel::BodyPanel(App& app) : LocalizedFloatPanel(app, 400, 600, L"bodyPanelTitle")
{
    layout = std::make_unique<VerticalControlList>();
    editorLayout = &layout->AddColumn();
    BuildEditor();
    editorLayout->SetVisibleCount(0, app);

    layout->AddRichLabel(0, GetDetailsText);
    contentControl = layout.get();
    Init(app);
}

void BodyPanel::BuildEditor()
{
    connectionList = &editorLayout->AddColumn();
    auto& scriptEditor = editorLayout->AddRichTextBox(360, 400);
    scriptEditor.valueProvider = [](App& app)
    {
        return static_cast<SimulationApp&>(app).editor.bodySelecting->scriptData.temporaryScriptText;
    };
    scriptEditor.onEditing = [](App& app, std::wstring const& text)
    {
        static_cast<SimulationApp&>(app).editor.bodySelecting->scriptData.temporaryScriptText = text;
    };

    BuildActionButtons();
    editorLayout->AddSeparator(400);
}

void BodyPanel::BuildActionButtons()
{
    auto& actionButtons = editorLayout->AddRow();
    actionButtons.AddLocalizedButton(70, L"applyBodyButton", [](App& app)
    {
        auto& simulationApp = static_cast<SimulationApp&>(app);
        auto& selectedBody = *simulationApp.editor.bodySelecting;
        selectedBody.scriptData.scriptText = selectedBody.scriptData.temporaryScriptText;
        simulationApp.scriptRuntime.RefreshBodyScript(selectedBody);
    });
    actionButtons.AddLocalizedButton(70, L"deleteBodyButton", [](App& app)
    {
        auto& simulationApp = static_cast<SimulationApp&>(app);
        simulationApp.editor.RequestBodyDeletion(simulationApp.physicsWorld);
    });
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
    editorLayout->SetVisibleCount(selectedBody ? editorLayout->GetSize() : 0, app);
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
    editorLayout->SetVisibleCount(0, app);
}

void BodyPanel::CreateConnectionRow(int id)
{
    auto& row = connectionList->AddRow();
    auto& connectionLabel = row.AddLabel(270);
    connectionLabel.textProvider = [id](App& app)
    {
        return GetConnectionText(app, id);
    };

    auto& selectCheckBox = row.AddLocalizedCheckBox(L"selectBodyConnection");
    selectCheckBox.checkedProvider = [id](App& app)
    {
        auto& editor = static_cast<SimulationApp&>(app).editor;
        return editor.connectionSelecting == editor.bodySelecting->GetConnections()[id];
    };
    selectCheckBox.onCheckedChanged = [id](App& app, bool checked)
    {
        auto& editor = static_cast<SimulationApp&>(app).editor;
        editor.connectionSelecting = checked ? editor.bodySelecting->GetConnections()[id] : nullptr;
    };
}

void BodyPanel::SyncConnectionRows(App& app)
{
    auto size = selectedBody->GetConnections().size();
    for (auto id = connectionList->GetSize(); id < size; ++id)
    {
        CreateConnectionRow(static_cast<int>(id));
    }
    connectionList->SetVisibleCount(size, app);
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
