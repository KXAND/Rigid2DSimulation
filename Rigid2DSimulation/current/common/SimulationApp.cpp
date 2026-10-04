#include "SimulationApp.h"
#include "UserInterface.h"
#include "SceneCanvas.h"
#include "Body.h"
#include "AudioClip.h"
#include "Creator.h"
#include "SceneStorage.h"

SimulationApp::SimulationApp()
{
    width = 1800;
    height = 900;
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
    currentMusicClip = GenerateRandomFloat(1) < 0.5 ? &*firstMusicClip : &*secondMusicClip;
}

void SimulationApp::Save(wstring const& name) const
{
    SceneStorage::Save(name, physicsWorld, scriptRuntime);
}

void SimulationApp::Load(wstring const& name)
{
    SceneStorage::Load(name, physicsWorld, scriptRuntime, [this]()
    {
        Reset();
    });
}
void SimulationApp::Reset()
{
    scriptRuntime.ResetScope({physicsWorld, editor, sceneRenderer, *sceneCanvas,
                              volume, maxRealDeltaTime, isTrackShown});

    physicsWorld.ClearObjects();
    scriptRuntime.ResetSceneVariables(*sceneCanvas);

    editor.ClearSelection();
    physicsWorld.ResetParameters(DRect(sceneCanvas->topLeft, sceneCanvas->width, sceneCanvas->height));
    maxRealDeltaTime = 0.05;
    sceneRenderer.energyMultiplier = 4e-6;
    sceneRenderer.chargeMultiplier = 1;
    isTrackShown = false;

    editor.ResetTools();
}

void SimulationApp::Update()
{
    ResolveInputOwnership();

    title = GetLocalizedString(L"title");
    scriptRuntime.Update();

    UpdateMusic();

    // 检查暂停
    if (!inputOwners.keyboardOwner)
    {
        if (input.IsKeyPressedDown(L' '))
        {
            isPaused = !isPaused;
        }
    }

    RemoveDeletedObjects();

    if (sceneCanvas->IsEmptyAreaClicked(input, inputOwners))
    {
        editor.ClearSelection();
    }

    // 保持底图与调试文字在物体更新、绘制之前输出。
    sceneRenderer.RenderBackground(*sceneCanvas, scriptRuntime.debugString, GetSceneRenderContext());

    PrepareSimulation();

    editor.UpdateBodies(physicsWorld, input, inputOwners, realDeltaTime, isTrackShown, isPaused);

    AdvanceSimulation();

    sceneRenderer.RenderObjects(physicsWorld, GetSceneRenderContext());

    if (editor.mode == kCreateMode)
    {
        editor.creator->Update(physicsWorld, input, inputOwners);
        sceneRenderer.RenderCreatorPreview(*editor.creator, GetSceneRenderContext());
    }
    userInterface->UpdateAndRender(*this);
}

void SimulationApp::ResolveInputOwnership()
{
    sceneCanvas->PreUpdate(input, inputOwners);
    editor.PreUpdate(physicsWorld, input, inputOwners, sceneCanvas->GetViewport());
    userInterface->PreUpdate(*this);
}

void SimulationApp::UpdateMusic()
{
    // 音乐
    if (firstMusicClip && secondMusicClip)
    {
        if (currentMusicClip->sampleIndex >= currentMusicClip->GetSampleCount())
        {
            currentMusicClip->sampleIndex = 0;
            currentMusicClip = (currentMusicClip == &*firstMusicClip) ? &*secondMusicClip : &*firstMusicClip;
        }
        if (!isMuted)
        {
            currentMusicClip->Play(waveOutput.inputSamples);
        }
    }
}

void SimulationApp::RemoveDeletedObjects()
{
    physicsWorld.MarkDeletedConnections();
    editor.ClearDeletedSelection();
    physicsWorld.RemoveDeletedObjects();
}

void SimulationApp::PrepareSimulation()
{
    // 更新运行信息
    realDeltaTime = min(maxRealDeltaTime, frameRate.GetDeltaTime());
    scriptRuntime.SetFrameDeltaTime(realDeltaTime);
    physicsWorld.Prepare();
}

void SimulationApp::AdvanceSimulation()
{
    if (!isPaused && realDeltaTime != 0)
    {
        physicsWorld.Advance(realDeltaTime,
            [this](double time)
            {
                scriptRuntime.SetSimulationTime(time);
            },
            [this](Body& body, double stepDeltaTime)
            {
                body.Step(physicsWorld.gravity, stepDeltaTime, [this, &body, stepDeltaTime](bool isDynamic)
                {
                    scriptRuntime.ApplyMotionPresets(body, isDynamic, physicsWorld.simulationTime, stepDeltaTime);
                });
                editor.ApplyDrag(body, input, inputOwners, realDeltaTime, stepDeltaTime);
                body.UpdateShapesPositionAndAABB();
            });
    }
}


SceneRenderContext SimulationApp::GetSceneRenderContext()
{
    return {screen, depthBuffer, font, sceneCanvas->GetViewport(), input.mousePosition,
            editor.bodySelecting, editor.connectionSelecting};
}
