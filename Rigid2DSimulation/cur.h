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
struct param;
struct Cur : App
{
	ptr<UI> ui;
	ptr<Background> mBgr;

	Scope curScope;
	wstring dbstr, cmd, tmp_cmd;
	vector<ptr<param>> pars;

	ptr<clip> cl0, cl1;
	clip* mClipPtr = NULL;
	double vol = 0;

	int mode = 0;
	int display = 0;
	double energy_mul = 0;
	double chargeMultiplier = 0;
	bool isTrackShown = false;
	bool isMuted = false;
	bool isPaused = false;
	bool isSceneChanged = false;
	vector2 gravity;
	double coulomb = 0;
	bool hasElectrostatic = false;
	double max_real_dt = 0;
	double real_dt = 0, t = 0;
	int stepNum = 0;
	dRect rectScene;
	ptr<Creator> creator;
	Body* bodySelecting = NULL;
	Connection* connectionSelecting = NULL;
	vector<ptr<Body>> bodies;
	vector<ptr<Group>> groups;
	vector<ptr<Collision>> collisions;
	vector<ptr<Connection>> connections;

	double eps_paralell = 0;
	bool equal_repos = false;
	double velocityAngularDrag = 0;

	double gridSize = 0;
	int gridNumX = 0, gridNumY = 0;
	vector<Body*> out_grid;
	vector<vector<Body*>> grid;

	Cur();
	void Save(wstring const& nm) const;
	void Load(wstring const& nm);
	void Reset();
	void Update() override;

	void set_cfg(Var const& v);
	void CollideBodies();

	void save_par(FILE* f) const;
	void load_par(FILE* f);
	void init_def_fun();
	void basicUpdate();
};
