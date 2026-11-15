#include "SimulationApp.h"
#include "UserInterface.h"
#include "SceneCanvas.h"
#include "Body.h"
#include "BodyDrag.h"
#include "Connection.h"
#include "SceneStorage.h"

SimulationApp::SimulationApp()
{
    width = 1800;
    height = 900;
    titleLocalizationId = L"title";
    SetLocale();
    waveOutput.blockDuration = 4 * 5e-4;

    PrintConsole(L"正在加载字体...");
    App::Init();

    PrintConsole(L"字体加载完成.", true);

    PrintConsole(L"正在加载控件...");
    userInterface = std::make_shared<UserInterface>(*this);
    userInterface->Init(*this);
    sceneCanvas = std::make_shared<SceneCanvas>();
    Reset();

    PrintConsole(L"控件加载完成.", true);

    PrintConsole(L"正在加载音乐...");
    music.Initialize();
    PrintConsole(L"音乐加载完成.", true);
}

void SimulationApp::Save(wstring const& name) const
{
    SceneStorage::Save(name, physicsWorld);
}

void SimulationApp::Load(wstring const& name)
{
    bool loaded = SceneStorage::Load(name, physicsWorld, [this]()
        {
            Reset();
        });
    if (!loaded)
    {
        AddLog(L"Scene loading did not complete: " + name + L"\n");
    }
}
void SimulationApp::Reset()
{
    editor.ResetInteraction();
    // UI 回调可能在本帧拾取之后重置场景，销毁前清除旧对象的输入归属。
    for (auto const& body : physicsWorld.GetBodies())
    {
        inputOwners.Remove(body.get());
    }
    for (auto const& connection : physicsWorld.GetConnections())
    {
        inputOwners.Remove(connection.get());
    }
    physicsWorld.ClearObjects();

    physicsWorld.ResetParameters(DRect(sceneCanvas->topLeft, sceneCanvas->width, sceneCanvas->height));
    maxRealDeltaTime = 0.05;
    sceneRenderer.energyMultiplier = 4e-6;
    sceneRenderer.chargeMultiplier = 1;
    isTrackShown = false;

    editor.ResetTools(inputOwners);
}

void SimulationApp::Update()
{
    // Resolve Input Ownership
    sceneCanvas->PreUpdate(input, inputOwners);
    editor.PreUpdate(physicsWorld, input, inputOwners, sceneCanvas->GetViewport());
    userInterface->PreUpdate(*this);
    HandleInput();
    // 按钮回调与快捷键都在推进物理之前生效；此处只更新控件，不绘制。
    userInterface->Update(*this);

    // 本帧编辑与物理推进共用同一时长。
    realDeltaTime = min(maxRealDeltaTime, frameRate.GetDeltaTime());

    music.Update(waveOutput.inputSamples);

    editor.Update(physicsWorld, input, inputOwners, *sceneCanvas, realDeltaTime);
    UpdateSceneState();
    AdvanceSimulation();
    auto context = GetSceneRenderContext();
    sceneRenderer.RenderBackground(*sceneCanvas, context);
    sceneRenderer.RenderObjects(physicsWorld, context);
    sceneRenderer.RenderCreationPreview(editor.GetPreview(), context);
    userInterface->Render(*this);
}

void SimulationApp::HandleInput()
{
    if (inputOwners.keyboardOwner)
    {
        return;
    }

    if (input.IsKeyPressedDown(L' '))
    {
        TogglePausing();
    }
}

void SimulationApp::TogglePausing()
{
    isPaused = !isPaused;
}

void SimulationApp::UpdateSceneState()
{
    // 先释放待删除对象的输入归属和编辑器引用，再实际移除对象。
    for (auto const& body : physicsWorld.GetBodies())
    {
        if (body->IsMarkedForDeletion())
        {
            inputOwners.Remove(body.get());
        }
    }
    for (auto const& connection : physicsWorld.GetConnections())
    {
        if (connection->IsMarkedForDeletion())
        {
            inputOwners.Remove(connection.get());
        }
    }
    editor.PrepareForDeletion(inputOwners);
    physicsWorld.RemoveDeletedObjects();

    physicsWorld.Prepare();
    for (auto const& body : physicsWorld.GetBodies())
    {
        ApplyBodyDragFrame(*body, editor.GetFrameDrag(), realDeltaTime);
    }
    editor.RecordTracks(physicsWorld, isTrackShown, isPaused);
}

void SimulationApp::AdvanceSimulation()
{
    auto drag = editor.GetDrag();
    if (!isPaused && realDeltaTime != 0)
    {
        physicsWorld.Advance(realDeltaTime, drag,
            [this](Body const& body)
            {
                editor.ClearTrack(body);
            });
    }
    else
    {
        // Advance 内部会同步几何；暂停或零时长不调用它，但编辑仍可能改变姿态。
        physicsWorld.UpdateGeometry();
    }
}


SceneRenderContext SimulationApp::GetSceneRenderContext()
{
    return { screen, depthBuffer, font, sceneCanvas->GetViewport(), input.mousePosition,
            editor.bodySelecting, editor.connectionSelecting,
            editor.GetHoveredBody(), editor.GetDrag().body, editor.GetDrag().relativePoint, editor.GetTracks() };
}
