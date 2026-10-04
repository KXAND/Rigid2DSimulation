#pragma once
#include "App.h"
#include "PhysicsWorld.h"
#include "Value.h"

int constexpr kDragMode = 0;
int constexpr kSelectMode = 1;
int constexpr kCreateMode = 2;
int constexpr kDeleteMode = 3;

int constexpr kColorDisplay = 0;
int constexpr kEnergyDisplay = 1;
int constexpr kChargesDisplay = 2;
int constexpr kNoDisplay = 3;

struct Body;
struct Creator;
struct Connection;

struct UserInterface;
struct SceneCanvas;
struct AudioClip;
struct SceneParameter;
struct SimulationApp : App
{
    std::shared_ptr<UserInterface> userInterface;
    std::shared_ptr<SceneCanvas> sceneCanvas;
    PhysicsWorld physicsWorld;

    Scope currentScope;
    wstring debugString, scriptText, temporaryScriptText;
    vector<std::shared_ptr<SceneParameter>> parameters;

    std::shared_ptr<AudioClip> firstMusicClip, secondMusicClip;
    AudioClip* currentMusicClip = NULL;
    double volume = 0;

    int mode = 0;
    int display = 0;
    double energyMultiplier = 0;
    double chargeMultiplier = 0;
    bool isTrackShown = false;
    bool isMuted = false;
    bool isPaused = false;
    double maxRealDeltaTime = 0;
    double realDeltaTime = 0;
    std::shared_ptr<Creator> creator;
    Body* bodySelecting = NULL;
    Connection* connectionSelecting = NULL;

    double velocityAngularDrag = 0;

    SimulationApp();
    void Save(wstring const& name) const;
    void Load(wstring const& name);
    void Reset();
    void ClearSelection();
    void Update() override;

    void SetConfig(Value const& config);

    void SaveParameters(FILE* file) const;
    void LoadParameters(FILE* file);
    void InitializeDefaultFunctions();
    void UpdateBasicState();

private:
    void ResolveInputOwnership();
    void UpdateMusic();
    void RemoveDeletedObjects();
    void PrepareSimulation();
    void AdvanceSimulation();
    void RenderSceneObjects();
};
