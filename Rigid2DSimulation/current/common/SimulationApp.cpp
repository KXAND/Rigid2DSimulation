#include "SimulationApp.h"
#include "UserInterface.h"
#include "SceneCanvas.h"
#include "Body.h"
#include "BodyDrag.h"
#include "Connection.h"
#include "AudioClip.h"
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
    bool audioLoaded = false;
    firstMusicClip = std::make_shared<AudioClip>(L"sound/bgm0.clip", &audioLoaded);
    if (!audioLoaded)
    {
        firstMusicClip = NULL;
    }
    secondMusicClip = std::make_shared<AudioClip>(L"sound/bgm1.clip", &audioLoaded);
    if (!audioLoaded)
    {
        secondMusicClip = NULL;
    }
    PrintConsole(L"音乐加载完成.", true);

    volume = 1;
    currentMusicClip = GenerateRandomFloat(1) < 0.5 ? firstMusicClip.get() : secondMusicClip.get();
}

void SimulationApp::Save(wstring const& name) const
{
    SceneStorage::Save(name, physicsWorld, scriptRuntime);
}

void SimulationApp::Load(wstring const& name)
{
    bool loaded = SceneStorage::Load(name, physicsWorld, scriptRuntime, [this]()
        {
            Reset();
        });
    if (!loaded)
    {
        AddLog(L"Scene loading did not complete: " + name + L"\n");
    }
    HandleScriptError();
}
void SimulationApp::Reset()
{
    scriptRuntime.ResetScope({ physicsWorld, editor, sceneRenderer, *sceneCanvas,
                              volume, maxRealDeltaTime, isTrackShown });

    editor.ResetInteraction();
    physicsWorld.ClearObjects();
    scriptRuntime.ResetSceneVariables(*sceneCanvas);

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

    // 本帧编辑与物理推进共用同一时长；暂不接入脚本的自动更新。
    realDeltaTime = min(maxRealDeltaTime, frameRate.GetDeltaTime());

    UpdateMusic();

    editor.Update(physicsWorld, input, inputOwners, *sceneCanvas, realDeltaTime);
    UpdateSceneState();
    AdvanceSimulation();
    auto context = GetSceneRenderContext();
    sceneRenderer.RenderBackground(*sceneCanvas, L"", context);
    sceneRenderer.RenderObjects(physicsWorld, context);
    sceneRenderer.RenderCreationPreview(editor.GetPreview(), context);
    userInterface->UpdateAndRender(*this);
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

void SimulationApp::HandleScriptError()
{
    if (!scriptRuntime.GetError().empty())
    {
        isPaused = true;
    }
    auto error = scriptRuntime.ConsumeError();
    if (!error.empty())
    {
        AddLog(L"Script error: " + error + L"\n");
    }
}

void SimulationApp::TogglePausing()
{
    isPaused = !isPaused;
}

void SimulationApp::UpdateMusic()
{
    // 音乐
    if (firstMusicClip && secondMusicClip)
    {
        if (currentMusicClip->sampleIndex >= currentMusicClip->GetSampleCount())
        {
            currentMusicClip->sampleIndex = 0;
            currentMusicClip = (currentMusicClip == firstMusicClip.get()) ? secondMusicClip.get() : firstMusicClip.get();
        }
        if (!isMuted)
        {
            currentMusicClip->Play(waveOutput.inputSamples);
        }
    }
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
        physicsWorld.Advance(realDeltaTime, drag.body,
            // 保留子步回调接口，但暂不发布脚本时间或执行运动预设。
            [](double) {},
            [this, &drag](Body& body, double stepDeltaTime)
            {
                body.Step(physicsWorld.gravity, stepDeltaTime, [](bool) {});
                ApplyBodyDragSubstep(body, drag, realDeltaTime, stepDeltaTime);
                body.UpdateShapesPositionAndAABB();
            },
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
