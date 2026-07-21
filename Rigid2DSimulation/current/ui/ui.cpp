#include "ui.h"
#include "SimulationApp.h"
#include "float_panel.h"
#include "fpn_global.h"
#include "fpn_body.h"
#include "fpn_con.h"
#include "fpn_par.h"
#include "fpn_cmd.h"
#include "fpn_lang.h"
#include "fpn_about.h"
#include "pn_menu.h"
#include "pn_bottom.h"


void UI::SortFloatPanels() { sort(fpns.begin(), fpns.end(), less_fpn); }
void UI::Init(SimulationApp& cur) {
	fpn_global = std::make_shared<FPnGlobal>(cur); fpns.push_back(&*fpn_global);
	fpn_body = std::make_shared<FPnBody>(cur); fpns.push_back(&*fpn_body);
	fpn_con = std::make_shared<FPnCon>(cur); fpns.push_back(&*fpn_con);
	fpn_cmd = std::make_shared<FPnCmd>(cur); fpns.push_back(&*fpn_cmd);
	fpn_par = std::make_shared<FPnPar>(cur); fpns.push_back(&*fpn_par);
	fpn_lang = std::make_shared<FPnLang>(cur); fpns.push_back(&*fpn_lang);
	fpn_about = std::make_shared<FPnAbout>(cur); fpns.push_back(&*fpn_about);

	// pn_menu 放在后面是因为要等 float_panel 先加载完。
	// pn_menu 放在后面是因为要等 float_panel 先加载完。
	// 这主要是 BtFPn 的原因。
	pn_menu = std::make_shared<PnMenu>(cur);
	pn_bottom = std::make_shared<PnBottom>(cur);
}
void UI::Update(SimulationApp& cur) {
	pn_menu->Update(cur);
	pn_bottom->Update(cur);
	for (auto fp : fpns) { fp->Update(cur); }
}
void UI::PreUpdate(SimulationApp& cur) {
	pn_menu->PreUpdate(cur);
	pn_bottom->PreUpdate(cur);
	SortFloatPanels();
	for (auto fp : fpns) { fp->PreUpdate(cur); }
}



