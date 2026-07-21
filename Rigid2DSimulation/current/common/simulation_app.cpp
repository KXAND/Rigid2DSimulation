#include "simulation_app.h"
#include "utils.h"
#include "ui.h"
#include "background.h"
#include "scene_parameter.h"
#include "shape.h"
#include "body.h"
#include "shape_builtins.h"
#include "scene_builders.h"
#include "AudioClip.h"
#include "group.h"
#include "creator.h"
#include "collision.h"
#include "connection.h"

SimulationApp::SimulationApp()
{
	w = 1800; h = 900; set_locale(); wv.len_block = 4 * 5e-4;
	print_console(L"正在加载字体...");
	App::Init();
	print_console(L"字体加载完成.", true);
	print_console(L"正在加载控件...");
	ui = std::make_shared<UI>(*this); ui->Init(*this);
	background = std::make_shared<Background>(*this); Reset();
	print_console(L"控件加载完成.", true);
	print_console(L"正在加载音乐...");
	bool ok = false;
	musicClip0 = std::make_shared<clip>(L"sound/bgm0.clip", &ok);
	if (!ok) { musicClip0 = NULL; }
	musicClip1 = std::make_shared<clip>(L"sound/bgm1.clip", &ok);
	if (!ok) { musicClip1 = NULL; }
	print_console(L"音乐加载完成.", true);
	vol = 1;
	currentMusicClip = GenerateRandomFloat(1) < 0.5 ? &*musicClip0 : &*musicClip1;
}

void SimulationApp::Save(wstring const& name) const
{
	FILE* file = wfopen(name, L"wb");
	if (!file) { return; }
	int size = 0;
	size = (scriptText).size();
	fwrite(&(size), sizeof(size), 1, file);
	fwrite(&*(scriptText).begin(), sizeof(*(&*(scriptText).begin())), size, file);
	SaveParameters(file);

	// 可能会保存一些即将被删除的元素，但是也无所谓。
	size = bodies.size();
	fwrite(&(size), sizeof(size), 1, file);
	for (auto b : bodies) { b->Save(file); }
	size = connections.size();
	fwrite(&(size), sizeof(size), 1, file);
	for (auto c : connections) { c->Save(*this, file); }

	fclose(file);
}
void SimulationApp::Load(wstring const& name)
{
	FILE* file = wfopen(name, L"rb");
	if (!file) { return; } Reset();
	int size = 0;
	fread(&(size), sizeof(size), 1, file);
	(scriptText).resize(size);
	fread(&*(scriptText).begin(), sizeof(*(&*(scriptText).begin())), size, file);
	temporaryScriptText = scriptText; LoadParameters(file);

	fread(&(size), sizeof(size), 1, file);
	for (int i = (0), iEnd = int(size); i < iEnd; ++i) { bodies.push_back(std::make_shared<Body>(*this, file)); }
	fread(&(size), sizeof(size), 1, file);
	for (int i = (0), iEnd = int(size); i < iEnd; ++i) { connections.push_back(std::make_shared<Connection>(*this, file)); }
	isSceneChanged = true;

	Execute(currentScope, Compile(scriptText)); fclose(file);
}
void SimulationApp::Reset()
{
	currentScope.clear(); InitializeDefaultFunctions(); parameters.clear();

	bodies.clear();
	groups.clear();
	collisions.clear();
	connections.clear();
	debugString.clear();

	currentScope[L"backgroundLeft"] = std::make_shared<Var>(background->topLeft.x);
	currentScope[L"backgroundTop"] = std::make_shared<Var>(background->topLeft.y);
	currentScope[L"backgroundWidth"] = std::make_shared<Var>(background->width);
	currentScope[L"backgroundHeight"] = std::make_shared<Var>(background->height);

	bodySelecting = NULL;
	connectionSelecting = NULL;
	isSceneChanged = false;
	gravity = Vector2();
	maxRealDeltaTime = 0.05;
	t = 0; stepNum = 60;
	rectScene.topLeftPosition = background->topLeft;
	rectScene.w = background->width;
	rectScene.h = background->height;
	gridSize = 50;
	energyMultiplier = 4e-6;
	chargeMultiplier = 1;
	isTrackShown = false;

	hasElectrostatic = false;
	coulomb = 1e5;

	parallelEpsilon = 1e-3;
	useEqualPositionCorrection = false;
	velocityAngularDrag = 2;

	creator = std::make_shared<Creator>();
}
void SimulationApp::Update()
{
	// pre update
	(*background).PreUpdate(*this);
	if (mode == MODE_CREATE) { creator->PreUpdate(*this); }
	for (auto& b : bodies) { b->PreUpdate(*this); }
	ui->PreUpdate(*this);
	UpdateBasicState();

	if (currentScope.find(L"Update") != currentScope.end())
	{
		auto tmp = *currentScope[L"Update"];
		Execute(currentScope, tmp.procs);
	}
	
	// 音乐
	if (musicClip0 && musicClip1)
	{
		if (currentMusicClip->csp >= currentMusicClip->n())
		{
			currentMusicClip->csp = 0;
			currentMusicClip = (currentMusicClip == &*musicClip0) ? &*musicClip1 : &*musicClip0;
		}
		if (!isMuted) { currentMusicClip->play(wv.wvin); }
	}

	// 检查暂停
	if (!own.keyboardOwner)
	{
		if (input.isKeyPressedDown(L' ')) { isPaused = !isPaused; }
	}

	// 删除被标记为删除的图形和其上的连接
	for (auto b : bodies) if (b->isMarkedForDeletion)
		for (auto c : b->connections) { c->isMarkedForDeletion = true; }
	if (bodySelecting && bodySelecting->isMarkedForDeletion) { bodySelecting = NULL; }
	if (connectionSelecting && connectionSelecting->isMarkedForDeletion) { connectionSelecting = NULL; }
	for (auto& c : connections) if (c->isMarkedForDeletion)
	{
		//将该连接从其两端的物体的连接列表中移除。
		auto& cons0 = c->body0->connections;
		cons0.erase(remove(cons0.begin(), cons0.end(), &*c), cons0.end());
		auto& cons1 = c->body1->connections;
		cons1.erase(remove(cons1.begin(), cons1.end(), &*c), cons1.end());
	}
	connections.erase(
		remove_if(connections.begin(), connections.end(),
			[](std::shared_ptr<Connection> c) { return c->isMarkedForDeletion; }),
		connections.end());
	bodies.erase(
		remove_if(bodies.begin(), bodies.end(),
			[](std::shared_ptr<Body> b) { return b->isMarkedForDeletion; }),
		bodies.end());
	
	// 更新运行信息
	(*background).Update(*this);
	(*background).Render(*this);
	realDeltaTime = min(maxRealDeltaTime, fps.dt());
	currentScope[L"cur.fps.dt()"] = std::make_shared<Var>(realDeltaTime);
	gridNumX = ceil(rectScene.w / gridSize);
	gridNumY = ceil(rectScene.h / gridSize);
	if (isSceneChanged) { isSceneChanged = false; groups = FormGroups(bodies); }
	for (const auto& b : bodies) { b->Update(*this); }

	// 物理模拟
	if (!isPaused && realDeltaTime != 0)
	{
		// 标准时长
		double sdt = realDeltaTime / stepNum;

		for (int i = (0), iEnd = int(stepNum); i < iEnd; ++i)
		{
			currentScope[L"time"] = std::make_shared<Var>(t);
			for (const auto& b : bodies) { b->Step(*this, sdt); }
			// for (auto b : bodies) { b->warp(rectScene); }
			for (auto& g : groups) { g->Warp(rectScene); }

			//计算静电
			if (hasElectrostatic)
			{
				for (int i = (0), iEnd = int(bodies.size() - 1); i < iEnd; ++i)
					for (int j = (i + 1), jEnd = int(bodies.size()); j < jEnd; ++j)
				{
					Electrostatic(*bodies[i], *bodies[j], sdt, coulomb);
				}
			}

			// 重新检查物体在grid中的情况
			multiGridBodies.clear();
			grid.clear();
			grid.resize(gridNumX * gridNumY);
			for (auto& b : bodies) { b->RegisterGrid(*this); }

			// 碰撞
			collisions.clear();
			DetectCollisions();
			for (auto& collision : collisions) { collision->Simulate(useEqualPositionCorrection); }
			// for (auto collision : collisions) { collision->Render(*this); }

			// 连接
			for (auto& connnection : connections) { connnection->Simulate(sdt, useEqualPositionCorrection); }

			t += sdt;
		}
	}

	// 渲染
	for (auto& b : bodies)
	{
		b->UpdateShapesPositionAndAABB();
		b->Render(*this);
	}
	for (auto& connection : connections)
	{
		connection->UpdatePosition();
		connection->Render(*this);
	}

	if (mode == MODE_CREATE) { creator->Update(*this); }
	ui->Update(*this);
}

void SimulationApp::SetCfg(Var const& v)
{
	TryReadConfigNumber(v, L"volume", vol);
	TryReadConfigNumber(v, L"energyMultiplier", energyMultiplier);
	TryReadConfigNumber(v, L"chargeMultiplier", chargeMultiplier);
	TryReadConfigNumber(v, L"parallelEpsilon", parallelEpsilon);
	TryReadConfigNumber(v, L"maxRealDeltaTime", maxRealDeltaTime);
	TryReadConfigNumber(v, L"hasElectrostatic", hasElectrostatic);
	TryReadConfigNumber(v, L"coulomb", coulomb);
	TryReadConfigNumber(v, L"stepCount", stepNum);
	stepNum = max(1, stepNum);
	TryReadConfigNumber(v, L"gridSize", gridSize);
	TryReadConfigNumber(v, L"useEqualPositionCorrection", useEqualPositionCorrection);
	TryReadConfigNumber(v, L"time", t);
	TryReadConfigNumber(v, L"isTrackShown", isTrackShown);
	if (auto value = FindConfigValue(v, L"sceneLeft")) { rectScene.topLeftPosition.x = value->num; }
	if (auto value = FindConfigValue(v, L"sceneTop")) { rectScene.topLeftPosition.y = value->num; }
	if (auto value = FindConfigValue(v, L"sceneWidth")) { rectScene.w = value->num; }
	if (auto value = FindConfigValue(v, L"sceneHeight")) { rectScene.h = value->num; }
	if (auto value = FindConfigValue(v, L"gravity")) { gravity = ToVector2(*value); }

	isSceneChanged = true;
}

void SimulationApp::DetectCollisions()
{
	for (int i = (0), iEnd = int(gridNumX); i < iEnd; ++i)
		for (int j = (0), jEnd = int(gridNumY); j < jEnd; ++j)
	{
		auto& bodies = grid[j * gridNumX + i];
		for (int k = (0), kEnd = int(bodies.size()); k < kEnd; ++k)
		{
			auto& bo = *bodies[k];

			// 检查同grid和跨grid的情况，输出到collisions中
			for (int m = (k + 1), mEnd = int(bodies.size()); m < mEnd; ++m) { Collide(bo, *bodies[m], collisions, parallelEpsilon); }
			for (auto pb1 : multiGridBodies) { Collide(bo, *pb1, collisions, parallelEpsilon); }

			// 检查右邻
			if (i + 1 < gridNumX)
			{
				int a = i + 1, b = j;
				for (auto pb1 : grid[b * gridNumX + a]) { Collide(bo, *pb1, collisions, parallelEpsilon); }
			}
			if (j + 1 >= gridNumY) { continue; }// 检查下邻，没有下邻就无法进行下列操作了

			//检查左下
			if (i - 1 >= 0)
			{
				int a = i - 1, b = j + 1;
				for (auto pb1 : grid[b * gridNumX + a]) { Collide(bo, *pb1, collisions, parallelEpsilon); }
			}
			// 检查右下
			if (i + 1 < gridNumX)
			{
				int a = i + 1, b = j + 1;
				for (auto pb1 : grid[b * gridNumX + a]) { Collide(bo, *pb1, collisions, parallelEpsilon); }
			}
			// 检查正下
			int a = i, b = j + 1;
			for (auto pb1 : grid[b * gridNumX + a]) { Collide(bo, *pb1, collisions, parallelEpsilon); }
		}
	}

	// 检查多gird和多gird本身的碰撞
	for (int i = (0), iEnd = int(multiGridBodies.size() - 1); i < iEnd; ++i)
		for (int j = (i + 1), jEnd = int(multiGridBodies.size()); j < jEnd; ++j)
	{
		Collide(*multiGridBodies[i], *multiGridBodies[j], collisions, parallelEpsilon);
	}
}

void SimulationApp::SaveParameters(FILE* file) const
{
	int size = parameters.size();
	fwrite(&(size), sizeof(size), 1, file);
	for (auto p : parameters) { p->Save(file); }
}
void SimulationApp::LoadParameters(FILE* file)
{
	int size = 0;
	fread(&(size), sizeof(size), 1, file);
	for (int i = (0), iEnd = int(size); i < iEnd; ++i) { parameters.push_back(std::make_shared<SceneParameter>(file)); }
}
void SimulationApp::InitializeDefaultFunctions()
{
	auto f0 = [this](vector<std::shared_ptr<Var>> const& in)
		{
			if (in.size() >= 1) { creator = std::make_shared<Creator>(*in[0]); }
			return std::make_shared<Var>();
		};
	currentScope[L"SetCreator"] = std::make_shared<Var>(f0);

	auto f1 = [this](vector<std::shared_ptr<Var>> const& in)
		{
			if (in.size() >= 2) { CreateBody(*this, *in[0], *in[1]); }
			return std::make_shared<Var>();
		};
	currentScope[L"CreateBody"] = std::make_shared<Var>(f1);

	auto f2 = [this](vector<std::shared_ptr<Var>> const& in)
		{
			if (in.size() >= 2) { return Box(*in[0], *in[1]); }
			return std::make_shared<Var>();
		};
	currentScope[L"Box"] = std::make_shared<Var>(f2);

	auto f3 = [this](vector<std::shared_ptr<Var>> const& in)
		{
			if (in.size() >= 1) { return Ball(*in[0]); }
			return std::make_shared<Var>();
		};
	currentScope[L"Ball"] = std::make_shared<Var>(f3);

	auto f4 = [this](vector<std::shared_ptr<Var>> const& in)
		{
			if (in.size() >= 2) { return Parallelogram(*in[0], *in[1], *in[2]); }
			return std::make_shared<Var>();
		};
	currentScope[L"Parallelogram"] = std::make_shared<Var>(f4);

	auto f5 = [this](vector<std::shared_ptr<Var>> const& in)
		{
			if (in.size() >= 3) { return Trapezoid(*in[0], *in[1], *in[2]); }
			return std::make_shared<Var>();
		};
	currentScope[L"Trapezoid"] = std::make_shared<Var>(f5);

	auto f6 = [this](vector<std::shared_ptr<Var>> const& in)
		{
			if (in.size() >= 2) { return RegularPoly(*in[0], *in[1]); }
			return std::make_shared<Var>();
		};
	currentScope[L"RegularPolygon"] = std::make_shared<Var>(f6);

	auto f7 = [this](vector<std::shared_ptr<Var>> const& in)
		{
			if (in.size() >= 2) { return Cross(*in[0], *in[1]); }
			return std::make_shared<Var>();
		};
	currentScope[L"Cross"] = std::make_shared<Var>(f7);

	auto f8 = [this](vector<std::shared_ptr<Var>> const& in)
		{
			return std::make_shared<Var>(bodies.size());
		};
	currentScope[L"GetBodyCount"] = std::make_shared<Var>(f8);

	auto f9 = [this](vector<std::shared_ptr<Var>> const& in)
		{
			if (in.size() >= 1) { CreateConnection(*this, *in[0]); }
			return std::make_shared<Var>();
		};
	currentScope[L"CreateConnection"] = std::make_shared<Var>(f9);

	auto f10 = [this](vector<std::shared_ptr<Var>> const& in)
		{
			if (in.size() >= 1) { SetCfg(*in[0]); }
			return std::make_shared<Var>();
		};
	currentScope[L"SetConfig"] = std::make_shared<Var>(f10);

	auto f11 = [this](vector<std::shared_ptr<Var>> const& in)
		{
			if (in.size() >= 2) { Boundary(*this, *in[0], *in[1]); }
			return std::make_shared<Var>();
		};
	currentScope[L"Boundary"] = std::make_shared<Var>(f11);

	auto f12 = [this](vector<std::shared_ptr<Var>> const& in)
		{
			if (in.size() >= 3) { return FramedBox(*in[0], *in[1], *in[2]); }
			return std::make_shared<Var>();
		};
	currentScope[L"FramedBox"] = std::make_shared<Var>(f12);

	auto f13 = [this](vector<std::shared_ptr<Var>> const& in)
		{
			if (in.size() >= 4) { Gear(*this, *in[0], *in[1], *in[2], *in[3]); }
			return std::make_shared<Var>();
		};
	currentScope[L"Gear"] = std::make_shared<Var>(f13);

	auto f14 = [this](vector<std::shared_ptr<Var>> const& in)
		{
			if (in.size() >= 6)
			{
				Strand(*this, *in[0], *in[1], *in[2], *in[3], *in[4], *in[5]);
			}
			return std::make_shared<Var>();
		};
	currentScope[L"Strand"] = std::make_shared<Var>(f14);

	auto f15 = [this](vector<std::shared_ptr<Var>> const& in)
		{
			if (in.size() >= 6)
			{
				Necklace(*this, *in[0], *in[1], *in[2], *in[3], *in[4], *in[5]);
			}
			return std::make_shared<Var>();
		};
	currentScope[L"Necklace"] = std::make_shared<Var>(f15);
}
void SimulationApp::UpdateBasicState()
{
	title = getLocalizedString(L"title");
	if (currentScope.find(L"cur.debugString") != currentScope.end()) { debugString = currentScope[L"cur.debugString"]->str; }
	if (currentScope.find(L"Update") != currentScope.end()) { auto file = currentScope[L"Update"];  Execute(currentScope, file->procs); }

	// 删除标记为删除的键
	parameters.erase(
		remove_if(parameters.begin(), parameters.end(),
			[](std::shared_ptr<SceneParameter> p) { return p->isMarkedForDeletion; }),
		parameters.end());

	//更新对应键的值
	for (auto& p : parameters) { currentScope[p->name] = std::make_shared<Var>(p->value); }
}



