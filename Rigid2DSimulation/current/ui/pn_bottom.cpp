#include "pn_bottom.h"
#include "SimulationApp.h"

LbFPS::LbFPS() { w = 450; fixed_w = true; }
void LbFPS::Sync(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	txt = simulationApp.getLocalizedString(L"frm_tm") + FormatFixed2(simulationApp.fps.frm_time * 1000) +
		L", FPS: " + FormatFixed2(simulationApp.fps.fps);
}

LbTime::LbTime() { w = 200; fixed_w = true; }
void LbTime::Sync(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	txt = simulationApp.getLocalizedString(L"lb_time") + FormatFixed2(simulationApp.t);
}
LbMouse::LbMouse() { w = 300; fixed_w = true; }
void LbMouse::Sync(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	txt = simulationApp.getLocalizedString(L"lb_mouse") + ToWString(simulationApp.input.msp);
}

PnBottom::PnBottom(App& app) : Panel(Panel::bottom(app)) {
	vector<Control*> tmp;
	lb_fps = std::make_shared<LbFPS>(); lb_time = std::make_shared<LbTime>(); lb_mouse = std::make_shared<LbMouse>();
	tmp = { &*lb_fps, &*lb_time, &*lb_mouse };
	clx = std::make_shared<CtrlListX>(tmp);
	c = &*clx;
	Init(app);
}



