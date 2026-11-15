#pragma once
#include "App.h"
#include "PhysicsWorld.h"
#include "SceneEditor.h"
#include "SceneRenderer.h"

class UserInterface;
class SceneCanvas;
class AudioClip;
class SimulationApp : public App
{
public:
    std::shared_ptr<UserInterface> userInterface;
    std::shared_ptr<SceneCanvas> sceneCanvas;
    PhysicsWorld physicsWorld;
    SceneEditor editor;
    SceneRenderer sceneRenderer;

    std::shared_ptr<AudioClip> firstMusicClip, secondMusicClip;
    AudioClip* currentMusicClip = NULL;
    double volume = 0;

    bool isTrackShown = false;
    bool isMuted = false;
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
    void UpdateMusic();
    // 暂停时也同步场景结构、应用每帧拖拽并维护轨迹，不推进模拟时间。
    void UpdateSceneState();
    void AdvanceSimulation();
    SceneRenderContext GetSceneRenderContext();
};
