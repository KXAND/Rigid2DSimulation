#include "UserInterface.h"
#include "SimulationApp.h"
#include "FloatPanel.h"
#include "GlobalPanel.h"
#include "BodyPanel.h"
#include "ConnectionPanel.h"
#include "ParameterPanel.h"
#include "CommandPanel.h"
#include "LanguagePanel.h"
#include "AboutPanel.h"
#include "MenuPanel.h"
#include "BottomPanel.h"


void UserInterface::SortFloatPanels() { sort(fpns.begin(), fpns.end(), less_fpn); }
void UserInterface::Init(SimulationApp& cur) {
	fpn_global = std::make_shared<GlobalPanel>(cur); fpns.push_back(&*fpn_global);
	fpn_body = std::make_shared<BodyPanel>(cur); fpns.push_back(&*fpn_body);
	fpn_con = std::make_shared<ConnectionPanel>(cur); fpns.push_back(&*fpn_con);
	fpn_cmd = std::make_shared<CommandPanel>(cur); fpns.push_back(&*fpn_cmd);
	fpn_par = std::make_shared<ParameterPanel>(cur); fpns.push_back(&*fpn_par);
	fpn_lang = std::make_shared<LanguagePanel>(cur); fpns.push_back(&*fpn_lang);
	fpn_about = std::make_shared<AboutPanel>(cur); fpns.push_back(&*fpn_about);

	// pn_menu 放在后面是因为要等 float_panel 先加载完。
	// pn_menu 放在后面是因为要等 float_panel 先加载完。
	// 这主要是 BtFPn 的原因。
	pn_menu = std::make_shared<MenuPanel>(cur);
	pn_bottom = std::make_shared<BottomPanel>(cur);
}
void UserInterface::Update(SimulationApp& cur) {
	pn_menu->Update(cur);
	pn_bottom->Update(cur);
	for (auto fp : fpns) { fp->Update(cur); }
}
void UserInterface::PreUpdate(SimulationApp& cur) {
	pn_menu->PreUpdate(cur);
	pn_bottom->PreUpdate(cur);
	SortFloatPanels();
	for (auto fp : fpns) { fp->PreUpdate(cur); }
}



