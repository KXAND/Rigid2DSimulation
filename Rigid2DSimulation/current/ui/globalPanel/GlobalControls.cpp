#include "GlobalControls.h"

#include "Creator.h"
#include "SimulationApp.h"

void GlobalDetailsLabel::Sync(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    text = L"";
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

    wstring modeName;
    switch (simulationApp.creator->mode)
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
}
