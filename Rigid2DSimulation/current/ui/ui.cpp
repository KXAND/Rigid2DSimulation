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

#define mkfp(x) mkp(x)(cur); fpns.push_back(&*x);

void UI::sort_fpns() { sort(fpns.begin(), fpns.end(), less_fpn); }
void UI::Init(SimulationApp& cur) {
	mkfp(fpn_global);
	mkfp(fpn_body);
	mkfp(fpn_con);
	mkfp(fpn_cmd);
	mkfp(fpn_par);
	mkfp(fpn_lang);
	mkfp(fpn_about);

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
	sort_fpns();
	for (auto fp : fpns) { fp->PreUpdate(cur); }
}



