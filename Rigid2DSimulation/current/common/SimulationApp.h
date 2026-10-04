#pragma once
#include "App.h"
#include "PhysicsWorld.h"
#include "SceneEditor.h"
#include "SceneRenderer.h"
#include "SceneScriptRuntime.h"

struct UserInterface;
struct SceneCanvas;
struct AudioClip;
struct SimulationApp : App
{
    std::shared_ptr<UserInterface> userInterface;
    std::shared_ptr<SceneCanvas> sceneCanvas;
    PhysicsWorld physicsWorld;
    SceneEditor editor;
    SceneRenderer sceneRenderer;

    SceneScriptRuntime scriptRuntime;

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

private:
    void ResolveInputOwnership();
    void UpdateMusic();
    void RemoveDeletedObjects();
    void PrepareSimulation();
    void AdvanceSimulation();
    SceneRenderContext GetSceneRenderContext();
};
