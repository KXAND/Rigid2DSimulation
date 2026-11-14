#pragma once
#include "App.h"
#include "PhysicsWorld.h"
#include "SceneEditor.h"
#include "SceneRenderer.h"
#include "SceneScriptRuntime.h"
#include "current/audio/MusicPlayer.h"

class UserInterface;
class SceneCanvas;
class SimulationApp : public App
{
public:
    std::shared_ptr<UserInterface> userInterface;
    std::shared_ptr<SceneCanvas> sceneCanvas;
    PhysicsWorld physicsWorld;
    SceneEditor editor;
    SceneRenderer sceneRenderer;

    SceneScriptRuntime scriptRuntime;

    MusicPlayer music;

    bool isTrackShown = false;
    bool isPaused = false;
    double maxRealDeltaTime = 0;
    double realDeltaTime = 0;

    SimulationApp();
    void Save(wstring const& name) const;
    void Load(wstring const& name);
    void Reset();
    void Update() override;
    void HandleInput();
    void TogglePausing();

private:
    void HandleScriptError();
    // 暂停时也同步场景结构、应用每帧拖拽并维护轨迹，不推进模拟时间。
    void UpdateSceneState();
    void AdvanceSimulation();
    SceneRenderContext GetSceneRenderContext();
};
