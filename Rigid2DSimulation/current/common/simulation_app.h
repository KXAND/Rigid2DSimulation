#pragma once
#include "app.h"
#include "var.h"

int constexpr MODE_DRAG = 0;
int constexpr MODE_SEL = 1;
int constexpr MODE_CREATE = 2;
int constexpr MODE_DEL = 3;

int constexpr DISPLAY_COLOR = 0;
int constexpr DISPLAY_ENERGY = 1;
int constexpr DISPLAY_CHARGES = 2;
int constexpr DISPLAY_NONE = 3;


struct Body;
struct Group;
struct Creator;
struct Collision;
struct Connection;

struct UI;
struct Background;
struct clip;
struct SceneParameter;
struct SimulationApp : App
{
	std::shared_ptr<UI> ui;
	std::shared_ptr<Background> background;

	Scope currentScope;
	wstring debugString, scriptText, temporaryScriptText;
	vector<std::shared_ptr<SceneParameter>> parameters;

	std::shared_ptr<clip> musicClip0, musicClip1;
	clip* currentMusicClip = NULL;
	double vol = 0;

	int mode = 0;
	int display = 0;
	double energyMultiplier = 0;
	double chargeMultiplier = 0;
	bool isTrackShown = false;
	bool isMuted = false;
	bool isPaused = false;
	bool isSceneChanged = false;
	vector2 gravity;
	double coulomb = 0;
	bool hasElectrostatic = false;
	double maxRealDeltaTime = 0;
	double realDeltaTime = 0, t = 0;
	int stepNum = 0;
	dRect rectScene;
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
	void Update() override;

	void SetCfg(Var const& v);
	void DetectCollisions();

	void SaveParameters(FILE* f) const;
	void LoadParameters(FILE* f);
	void InitializeDefaultFunctions();
	void UpdateBasicState();
};


