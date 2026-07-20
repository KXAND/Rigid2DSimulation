#pragma once
#include "utils.h"

struct SimulationApp;
struct FloatPanel;
struct FPnGlobal;
struct FPnBody;
struct FPnCon;
struct FPnCmd;
struct FPnPar;
struct FPnLang;
struct FPnAbout;
struct PnMenu;
struct PnBottom;
struct UI {
	vector<FloatPanel*> fpns;
	std::shared_ptr<FPnGlobal> fpn_global;
	std::shared_ptr<FPnBody> fpn_body;
	std::shared_ptr<FPnCon> fpn_con;
	std::shared_ptr<FPnCmd> fpn_cmd;
	std::shared_ptr<FPnPar> fpn_par;
	std::shared_ptr<FPnLang> fpn_lang;
	std::shared_ptr<FPnAbout> fpn_about;
	std::shared_ptr<PnMenu> pn_menu;
	std::shared_ptr<PnBottom> pn_bottom;

	UI(SimulationApp& cur) {};
	void SortFloatPanels();

	// 这里用 init 的原因是控件会用 cur->ui 来访问 ui, 但这个时候 ui 尚未初始化。
	// 这也是为什么主程序不需要 Init。这应该是一个普遍的问题。
	// 好像上面这些话已经过时了，不过还是保留这个设计以防万一。
	void Init(SimulationApp& cur);
	void Update(SimulationApp& cur);
	void PreUpdate(SimulationApp& cur);
};


