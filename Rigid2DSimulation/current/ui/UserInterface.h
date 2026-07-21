#pragma once
#include "Utils.h"

struct SimulationApp;
struct FloatPanel;
struct GlobalPanel;
struct BodyPanel;
struct ConnectionPanel;
struct CommandPanel;
struct ParameterPanel;
struct LanguagePanel;
struct AboutPanel;
struct MenuPanel;
struct BottomPanel;
struct UserInterface {
	vector<FloatPanel*> fpns;
	std::shared_ptr<GlobalPanel> fpn_global;
	std::shared_ptr<BodyPanel> fpn_body;
	std::shared_ptr<ConnectionPanel> fpn_con;
	std::shared_ptr<CommandPanel> fpn_cmd;
	std::shared_ptr<ParameterPanel> fpn_par;
	std::shared_ptr<LanguagePanel> fpn_lang;
	std::shared_ptr<AboutPanel> fpn_about;
	std::shared_ptr<MenuPanel> pn_menu;
	std::shared_ptr<BottomPanel> pn_bottom;

	UserInterface(SimulationApp& cur) {};
	void SortFloatPanels();

	// 这里用 init 的原因是控件会用 cur->ui 来访问 ui, 但这个时候 ui 尚未初始化。
	// 这也是为什么主程序不需要 Init。这应该是一个普遍的问题。
	// 好像上面这些话已经过时了，不过还是保留这个设计以防万一。
	void Init(SimulationApp& cur);
	void Update(SimulationApp& cur);
	void PreUpdate(SimulationApp& cur);
};


