#include "ui.h"
#include "simulation_app.h"
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
	mkp(fpn_global)(cur); fpns.push_back(&*fpn_global);
	mkp(fpn_body)(cur); fpns.push_back(&*fpn_body);
	mkp(fpn_con)(cur); fpns.push_back(&*fpn_con);
	mkp(fpn_cmd)(cur); fpns.push_back(&*fpn_cmd);
	mkp(fpn_par)(cur); fpns.push_back(&*fpn_par);
	mkp(fpn_lang)(cur); fpns.push_back(&*fpn_lang);
	mkp(fpn_about)(cur); fpns.push_back(&*fpn_about);

	// pn_menu 放在后面是因为要等 float_panel 先加载完。
	// pn_menu 放在后面是因为要等 float_panel 先加载完。
	// 这主要是 BtFPn 的原因。
	mkp(pn_menu)(cur);
	mkp(pn_bottom)(cur);
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



