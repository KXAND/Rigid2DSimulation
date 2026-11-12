#include "GlobalPanel.h"

#include <memory>
#include <utility>

#include "Creator.h"
#include "SimulationApp.h"
#include "TextBox.h"
#include "UserInterface.h"

GlobalPanel::GlobalPanel(App& app) : LocalizedFloatPanel(app.screen.Rect(), 400, 600, L"globalPanelTitle")
{
    auto layout = std::make_unique<VerticalStack>();

    auto& saveRow = layout->AddRow();
    savePathTextBox = &saveRow.AddTextBox(270);
    saveRow.AddLocalizedButton(80, L"saveButton", SaveScene);

    auto& loadRow = layout->AddRow();
    loadPathTextBox = &loadRow.AddTextBox(270);
    loadRow.AddLocalizedButton(80, L"loadButton", LoadScene);

    layout->AddSeparator(400);
    // 详情仍在保存、加载按钮之后读取，保留原来的宽度策略。
    layout->AddRichLabel(0, GetDetailsText);

    SetContent(std::move(layout));
}

void GlobalPanel::SaveScene(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    simulationApp.Save(simulationApp.userInterface->globalPanel->savePathTextBox->value);
}

void GlobalPanel::LoadScene(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    simulationApp.Load(simulationApp.userInterface->globalPanel->loadPathTextBox->value);
}

std::wstring GlobalPanel::GetDetailsText(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    std::wstring text;
    text += simulationApp.GetLocalizedString(L"volume") + FormatFixed2(simulationApp.volume) + L"\n";
    text += simulationApp.GetLocalizedString(L"stepCount") + std::to_wstring(simulationApp.physicsWorld.stepNum) + L"\n";
    text += simulationApp.GetLocalizedString(L"gridSize") + FormatFixed2(simulationApp.physicsWorld.gridSize) + L"\n";
    text += simulationApp.GetLocalizedString(L"maxRealDeltaTime") + FormatFixed2(simulationApp.maxRealDeltaTime) + L"\n";
    text += simulationApp.GetLocalizedString(L"sceneTopLeft") + ToWString(simulationApp.physicsWorld.rectScene.topLeftPosition) + L"\n";
    text += simulationApp.GetLocalizedString(L"sceneWidth") + std::to_wstring(simulationApp.physicsWorld.rectScene.width) + L"\n";
    text += simulationApp.GetLocalizedString(L"sceneHeight") + std::to_wstring(simulationApp.physicsWorld.rectScene.height) + L"\n";
    text += simulationApp.GetLocalizedString(L"gravity") + ToWString(simulationApp.physicsWorld.gravity) + L"\n";
    text += simulationApp.GetLocalizedString(L"electrostatic") + std::to_wstring(simulationApp.physicsWorld.hasElectrostatic) + L"\n";
    text += simulationApp.GetLocalizedString(L"coulomb") + FormatFixed2(simulationApp.physicsWorld.coulombConstant) + L"\n";

    std::wstring modeName;
    switch (simulationApp.editor.GetCreator().GetConfig().mode)
    {
    case kCreateBall:
        modeName = simulationApp.GetLocalizedString(L"createBall");
        break;
    case kCreateBox:
        modeName = simulationApp.GetLocalizedString(L"createBox");
        break;
    case kCreateConnection:
        modeName = simulationApp.GetLocalizedString(L"createConnection");
        break;
    case kCreateNail:
        modeName = simulationApp.GetLocalizedString(L"createNail");
        break;
    case kCreateParticle:
        modeName = simulationApp.GetLocalizedString(L"createParticle");
        break;
    case kCreatePlate:
        modeName = simulationApp.GetLocalizedString(L"createPlate");
        break;
    case kCreatePoint:
        modeName = simulationApp.GetLocalizedString(L"createPoint");
        break;
    }
    text += simulationApp.GetLocalizedString(L"creatorMode") + modeName + L"\n";
    return text;
}
