#include "GlobalPanel.h"

#include <memory>
#include <vector>

#include "Creator.h"
#include "SimulationApp.h"
#include "UserInterface.h"

GlobalPanel::GlobalPanel(App& app) : LocalizedFloatPanel(app, 400, 600, L"globalPanelTitle")
{
    BuildSaveRow();
    BuildLoadRow();
    separator = std::make_shared<HorizontalSeparator>(400);

    detailsLabel = std::make_shared<RichLabel>();
    // 在保存、加载按钮更新后读取详情，保持同帧显示加载结果。
    detailsLabel->textProvider = GetDetailsText;

    layout = std::make_shared<VerticalControlList>(
        std::vector<Control*>{saveRow.get(), loadRow.get(), separator.get(), detailsLabel.get()});
    contentControl = layout.get();
    Init(app);
}

void GlobalPanel::BuildSaveRow()
{
    savePathTextBox = std::make_shared<TextBox>(270);
    saveButton = std::make_shared<LocalizedButton>(80, L"saveButton");
    saveButton->onClick = [](App& app)
    {
        auto& simulationApp = static_cast<SimulationApp&>(app);
        simulationApp.Save(simulationApp.userInterface->globalPanel->savePathTextBox->value);
    };
    saveRow = std::make_shared<HorizontalControlList>(
        std::vector<Control*>{savePathTextBox.get(), saveButton.get()});
}

void GlobalPanel::BuildLoadRow()
{
    loadPathTextBox = std::make_shared<TextBox>(270);
    loadButton = std::make_shared<LocalizedButton>(80, L"loadButton");
    loadButton->onClick = [](App& app)
    {
        auto& simulationApp = static_cast<SimulationApp&>(app);
        simulationApp.Load(simulationApp.userInterface->globalPanel->loadPathTextBox->value);
    };
    loadRow = std::make_shared<HorizontalControlList>(
        std::vector<Control*>{loadPathTextBox.get(), loadButton.get()});
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
