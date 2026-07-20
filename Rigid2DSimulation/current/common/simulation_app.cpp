#include "simulation_app.h"
#include "utils.h"
#include "ui.h"
#include "background.h"
#include "scene_parameter.h"
#include "shape.h"
#include "body.h"
#include "shape_builtins.h"
#include "scene_builders.h"
#include "clip.h"
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
	mkp(ui)(*this); ui->Init(*this);
	mkp(background)(*this); Reset();
	print_console(L"控件加载完成.", true);
	print_console(L"正在加载音乐...");
	bool ok = false;
	mkp(cl0)(L"sound/bgm0.clip", &ok);
	if (!ok) { cl0 = NULL; }
	mkp(cl1)(L"sound/bgm1.clip", &ok);
	if (!ok) { cl1 = NULL; }
	print_console(L"音乐加载完成.", true);
	vol = 1;
	mClipPtr = GenerateRandomFloat(1) < 0.5 ? &*cl0 : &*cl1;
}

void SimulationApp::Save(wstring const& nm) const
{
	FILE* f = wfopen(nm, L"wb");
	if (!f) { return; }
	int sz = 0; fwtv(scriptText); SaveParameters(f);

	// 可能会保存一些即将被删除的元素，但是也无所谓。
	sz = bodies.size(); fwt(sz);
	for (auto b : bodies) { b->Save(f); }
	sz = connections.size(); fwt(sz);
	for (auto c : connections) { c->Save(*this, f); }

	fclose(f);
}
void SimulationApp::Load(wstring const& nm)
{
	FILE* f = wfopen(nm, L"rb");
	if (!f) { return; } Reset();
	int sz = 0; frdv(scriptText);
	temporaryScriptText = scriptText; LoadParameters(f);

	frd(sz);
	rep(i, 0, sz) { bodies.push_back(msh<Body>(*this, f)); }
	frd(sz);
	rep(i, 0, sz) { connections.push_back(msh<Connection>(*this, f)); }
	isSceneChanged = true;

	Execute(curScope, Compile(scriptText)); fclose(f);
}
void SimulationApp::Reset()
{
	curScope.clear(); InitializeDefaultFunctions(); pars.clear();

	bodies.clear();
	groups.clear();
	collisions.clear();
	connections.clear();
	dbstr.clear();

	curScope[L"left_bgr"] = msh<Var>(background->topLeft.x);
	curScope[L"top_bgr"] = msh<Var>(background->topLeft.y);
	curScope[L"w_bgr"] = msh<Var>(background->width);
	curScope[L"h_bgr"] = msh<Var>(background->height);

	bodySelecting = NULL;
	connectionSelecting = NULL;
	isSceneChanged = false;
	gravity = vector2();
	max_real_dt = 0.05;
	t = 0; stepNum = 60;
	rectScene.topLeftPosition = background->topLeft;
	rectScene.w = background->width;
	rectScene.h = background->height;
	gridSize = 50;
	energy_mul = 4e-6;
	chargeMultiplier = 1;
	isTrackShown = false;

	hasElectrostatic = false;
	coulomb = 1e5;

	eps_paralell = 1e-3;
	useEqualPositionCorrection = false;
	velocityAngularDrag = 2;

	mkp(creator)();
}
void SimulationApp::Update()
{
	// pre update
	(*background).PreUpdate(*this);
	if (mode == MODE_CREATE) { creator->PreUpdate(*this); }
	for (auto& b : bodies) { b->PreUpdate(*this); }
	ui->PreUpdate(*this);
	UpdateBasicState();

	if (curScope.find(L"update") != curScope.end())
	{
		auto tmp = *curScope[L"update"];
		Execute(curScope, tmp.procs);
	}
	
	// 音乐
	if (cl0 && cl1)
	{
		if (mClipPtr->csp >= mClipPtr->n())
		{
			mClipPtr->csp = 0;
			mClipPtr = (mClipPtr == &*cl0) ? &*cl1 : &*cl0;
		}
		if (!isMuted) { mClipPtr->play(wv.wvin); }
	}

	// 检查暂停
	if (!own.keyboardOwner)
	{
		if (input.isKeyPressedDown(L' ')) { isPaused = !isPaused; }
	}

	// 删除被标记为删除的图形和其上的连接
	for (auto b : bodies) if (b->del)
		for (auto c : b->connections) { c->del = true; }
	if (bodySelecting && bodySelecting->del) { bodySelecting = NULL; }
	if (connectionSelecting && connectionSelecting->del) { connectionSelecting = NULL; }
	for (auto& c : connections) if (c->del)
	{
		//将该连接从其两端的物体的连接列表中移除。
		auto& cons0 = c->body0->connections;
		cons0.erase(remove(cons0.begin(), cons0.end(), &*c), cons0.end());
		auto& cons1 = c->body1->connections;
		cons1.erase(remove(cons1.begin(), cons1.end(), &*c), cons1.end());
	}
	connections.erase(
		remove_if(connections.begin(), connections.end(),
			[](ptr<Connection> c) { return c->del; }),
		connections.end());
	bodies.erase(
		remove_if(bodies.begin(), bodies.end(),
			[](ptr<Body> b) { return b->del; }),
		bodies.end());
	
	// 更新运行信息
	(*background).Update(*this);
	(*background).Render(*this);
	real_dt = min(max_real_dt, fps.dt());
	curScope[L"cur.fps.dt()"] = msh<Var>(real_dt);
	gridNumX = ceil(rectScene.w / gridSize);
	gridNumY = ceil(rectScene.h / gridSize);
	if (isSceneChanged) { isSceneChanged = false; groups = FormGroups(bodies); }
	for (const auto& b : bodies) { b->Update(*this); }

	// 物理模拟
	if (!isPaused && real_dt != 0)
	{
		// 标准时长
		double sdt = real_dt / stepNum;

		rep(i, 0, stepNum)
		{
			curScope[L"t"] = msh<Var>(t);
			for (const auto& b : bodies) { b->Step(*this, sdt); }
			// for (auto b : bodies) { b->warp(rectScene); }
			for (auto& g : groups) { g->Warp(rectScene); }

			//计算静电
			if (hasElectrostatic)
			{
				rep(i, 0, bodies.size() - 1)
					rep(j, i + 1, bodies.size())
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
			for (auto& collision : collisions) { collision->simulate(useEqualPositionCorrection); }
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
	TryReadConfigNumber(v, L"vol", vol);
	TryReadConfigNumber(v, L"energy_mul", energy_mul);
	TryReadConfigNumber(v, L"chargeMultiplier", chargeMultiplier);
	TryReadConfigNumber(v, L"eps_paralell", eps_paralell);
	TryReadConfigNumber(v, L"max_real_dt", max_real_dt);
	TryReadConfigNumber(v, L"hasElectrostatic", hasElectrostatic);
	TryReadConfigNumber(v, L"coulomb", coulomb);
	TryReadConfigNumber(v, L"stepNum", stepNum);
	stepNum = max(1, stepNum);
	TryReadConfigNumber(v, L"gridSize", gridSize);
	TryReadConfigNumber(v, L"useEqualPositionCorrection", useEqualPositionCorrection);
	TryReadConfigNumber(v, L"t", t);
	TryReadConfigNumber(v, L"isTrackShown", isTrackShown);
	if (auto value = FindConfigValue(v, L"left_scene")) { rectScene.topLeftPosition.x = value->num; }
	if (auto value = FindConfigValue(v, L"top_scene")) { rectScene.topLeftPosition.y = value->num; }
	if (auto value = FindConfigValue(v, L"w_scene")) { rectScene.w = value->num; }
	if (auto value = FindConfigValue(v, L"h_scene")) { rectScene.h = value->num; }
	if (auto value = FindConfigValue(v, L"gravity")) { gravity = tv2(*value); }

	isSceneChanged = true;
}

void SimulationApp::DetectCollisions()
{
	rep(i, 0, gridNumX)
		rep(j, 0, gridNumY)
	{
		auto& bodies = grid[j * gridNumX + i];
		rep(k, 0, bodies.size())
		{
			auto& bo = *bodies[k];

			// 检查同grid和跨grid的情况，输出到collisions中
			rep(m, k + 1, bodies.size()) { Collide(bo, *bodies[m], collisions, eps_paralell); }
			for (auto pb1 : multiGridBodies) { Collide(bo, *pb1, collisions, eps_paralell); }

			// 检查右邻
			if (i + 1 < gridNumX)
			{
				int a = i + 1, b = j;
				for (auto pb1 : grid[b * gridNumX + a]) { Collide(bo, *pb1, collisions, eps_paralell); }
			}
			if (j + 1 >= gridNumY) { continue; }// 检查下邻，没有下邻就无法进行下列操作了

			//检查左下
			if (i - 1 >= 0)
			{
				int a = i - 1, b = j + 1;
				for (auto pb1 : grid[b * gridNumX + a]) { Collide(bo, *pb1, collisions, eps_paralell); }
			}
			// 检查右下
			if (i + 1 < gridNumX)
			{
				int a = i + 1, b = j + 1;
				for (auto pb1 : grid[b * gridNumX + a]) { Collide(bo, *pb1, collisions, eps_paralell); }
			}
			// 检查正下
			int a = i, b = j + 1;
			for (auto pb1 : grid[b * gridNumX + a]) { Collide(bo, *pb1, collisions, eps_paralell); }
		}
	}

	// 检查多gird和多gird本身的碰撞
	rep(i, 0, multiGridBodies.size() - 1)
		rep(j, i + 1, multiGridBodies.size())
	{
		Collide(*multiGridBodies[i], *multiGridBodies[j], collisions, eps_paralell);
	}
}

void SimulationApp::SaveParameters(FILE* f) const
{
	int sz = pars.size(); fwt(sz);
	for (auto p : pars) { p->Save(f); }
}
void SimulationApp::LoadParameters(FILE* f)
{
	int sz = 0; frd(sz);
	rep(i, 0, sz) { pars.push_back(msh<SceneParameter>(f)); }
}
void SimulationApp::InitializeDefaultFunctions()
{
	auto f0 = [this](vector<ptr<Var>> const& in)
		{
			if (in.size() >= 1) { mkp(creator)(*in[0]); }
			return msh<Var>();
		};
	curScope[L"set_creator"] = msh<Var>(f0);

	auto f1 = [this](vector<ptr<Var>> const& in)
		{
			if (in.size() >= 2) { CreateBody(*this, *in[0], *in[1]); }
			return msh<Var>();
		};
	curScope[L"create_body"] = msh<Var>(f1);

	auto f2 = [this](vector<ptr<Var>> const& in)
		{
			if (in.size() >= 2) { return Box(*in[0], *in[1]); }
			return msh<Var>();
		};
	curScope[L"box"] = msh<Var>(f2);

	auto f3 = [this](vector<ptr<Var>> const& in)
		{
			if (in.size() >= 1) { return Ball(*in[0]); }
			return msh<Var>();
		};
	curScope[L"ball"] = msh<Var>(f3);

	auto f4 = [this](vector<ptr<Var>> const& in)
		{
			if (in.size() >= 2) { return Parallelogram(*in[0], *in[1], *in[2]); }
			return msh<Var>();
		};
	curScope[L"parallelogram"] = msh<Var>(f4);

	auto f5 = [this](vector<ptr<Var>> const& in)
		{
			if (in.size() >= 3) { return Trapezoid(*in[0], *in[1], *in[2]); }
			return msh<Var>();
		};
	curScope[L"trapezoid"] = msh<Var>(f5);

	auto f6 = [this](vector<ptr<Var>> const& in)
		{
			if (in.size() >= 2) { return RegularPoly(*in[0], *in[1]); }
			return msh<Var>();
		};
	curScope[L"regular_poly"] = msh<Var>(f6);

	auto f7 = [this](vector<ptr<Var>> const& in)
		{
			if (in.size() >= 2) { return Cross(*in[0], *in[1]); }
			return msh<Var>();
		};
	curScope[L"cross"] = msh<Var>(f7);

	auto f8 = [this](vector<ptr<Var>> const& in)
		{
			return msh<Var>(bodies.size());
		};
	curScope[L"n_bodies"] = msh<Var>(f8);

	auto f9 = [this](vector<ptr<Var>> const& in)
		{
			if (in.size() >= 1) { CreateConnection(*this, *in[0]); }
			return msh<Var>();
		};
	curScope[L"create_conn"] = msh<Var>(f9);

	auto f10 = [this](vector<ptr<Var>> const& in)
		{
			if (in.size() >= 1) { SetCfg(*in[0]); }
			return msh<Var>();
		};
	curScope[L"set_cfg"] = msh<Var>(f10);

	auto f11 = [this](vector<ptr<Var>> const& in)
		{
			if (in.size() >= 2) { Boundary(*this, *in[0], *in[1]); }
			return msh<Var>();
		};
	curScope[L"boundary"] = msh<Var>(f11);

	auto f12 = [this](vector<ptr<Var>> const& in)
		{
			if (in.size() >= 3) { return FramedBox(*in[0], *in[1], *in[2]); }
			return msh<Var>();
		};
	curScope[L"framed_box"] = msh<Var>(f12);

	auto f13 = [this](vector<ptr<Var>> const& in)
		{
			if (in.size() >= 4) { Gear(*this, *in[0], *in[1], *in[2], *in[3]); }
			return msh<Var>();
		};
	curScope[L"gear"] = msh<Var>(f13);

	auto f14 = [this](vector<ptr<Var>> const& in)
		{
			if (in.size() >= 6)
			{
				Strand(*this, *in[0], *in[1], *in[2], *in[3], *in[4], *in[5]);
			}
			return msh<Var>();
		};
	curScope[L"strand"] = msh<Var>(f14);

	auto f15 = [this](vector<ptr<Var>> const& in)
		{
			if (in.size() >= 6)
			{
				Necklace(*this, *in[0], *in[1], *in[2], *in[3], *in[4], *in[5]);
			}
			return msh<Var>();
		};
	curScope[L"necklace"] = msh<Var>(f15);
}
void SimulationApp::UpdateBasicState()
{
	title = getLocalizedString(L"title");
	if (curScope.find(L"cur.dbstr") != curScope.end()) { dbstr = curScope[L"cur.dbstr"]->str; }
	if (curScope.find(L"update") != curScope.end()) { auto f = curScope[L"update"];  Execute(curScope, f->procs); }

	// 删除标记为删除的键
	pars.erase(
		remove_if(pars.begin(), pars.end(),
			[](ptr<SceneParameter> p) { return p->del; }),
		pars.end());

	//更新对应键的值
	for (auto& p : pars) { curScope[p->nm] = msh<Var>(p->val); }
}



