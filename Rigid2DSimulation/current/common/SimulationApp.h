#pragma once
#include "App.h"
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
struct Group;
struct Creator;
struct Collision;
struct Connection;

struct UserInterface;
struct SceneCanvas;
struct AudioClip;
struct SceneParameter;
struct SimulationApp : App
{
    std::shared_ptr<UserInterface> userInterface;
    std::shared_ptr<SceneCanvas> sceneCanvas;

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
    bool isSceneChanged = false;
    Vector2 gravity;
    double coulombConstant = 0;
    bool hasElectrostatic = false;
    double maxRealDeltaTime = 0;
    double realDeltaTime = 0, simulationTime = 0;
    int stepNum = 0;
    DRect rectScene;
    std::shared_ptr<Creator> creator;
    Body* bodySelecting = NULL;
    Connection* connectionSelecting = NULL;
    vector<std::shared_ptr<Body>> bodies;
    vector<std::shared_ptr<Group>> groups;
    vector<std::shared_ptr<Collision>> collisions;
    vector<std::shared_ptr<Connection>> connections;

    double parallelEpsilon = 0;
    bool useEqualPositionCorrection = false;
    double velocityAngularDrag = 0;

    double gridSize = 0;
    int gridNumX = 0, gridNumY = 0;
    vector<Body*> multiGridBodies;
    vector<vector<Body*>> grid;

    SimulationApp();
    void Save(wstring const& name) const;
    void Load(wstring const& name);
    void Reset();
    void ClearSelection();
    void Update() override;

    void SetConfig(Value const& config);
    void DetectCollisions();

    void SaveParameters(FILE* file) const;
    void LoadParameters(FILE* file);
    void InitializeDefaultFunctions();
    void UpdateBasicState();
};
