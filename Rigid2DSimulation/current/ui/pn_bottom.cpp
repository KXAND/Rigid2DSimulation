#include "pn_bottom.h"
#include "simulation_app.h"

LbFPS::LbFPS() { w = 450; fixed_w = true; }
void LbFPS::Sync(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	txt = simulationApp.getLocalizedString(L"frm_tm") + tw2(simulationApp.fps.frm_time * 1000) +
		L", FPS: " + tw2(simulationApp.fps.fps);
}

LbTime::LbTime() { w = 200; fixed_w = true; }
void LbTime::Sync(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	txt = simulationApp.getLocalizedString(L"lb_time") + tw2(simulationApp.t);
}
LbMouse::LbMouse() { w = 300; fixed_w = true; }
void LbMouse::Sync(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	txt = simulationApp.getLocalizedString(L"lb_mouse") + tw(simulationApp.input.msp);
}

PnBottom::PnBottom(App& app) : Panel(Panel::bottom(app)) {
	vector<Control*> tmp;
	mkp(lb_fps)(); mkp(lb_time)(); mkp(lb_mouse)();
	tmp = { &*lb_fps, &*lb_time, &*lb_mouse };
	mkp(clx)(tmp);
	c = &*clx;
	Init(app);
}



