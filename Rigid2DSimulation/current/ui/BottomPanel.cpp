#include "BottomPanel.h"
#include "SimulationApp.h"

FrameRateLabel::FrameRateLabel() { w = 450; fixed_w = true; }
void FrameRateLabel::Sync(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	txt = simulationApp.getLocalizedString(L"frm_tm") + FormatFixed2(simulationApp.fps.frm_time * 1000) +
		L", FPS: " + FormatFixed2(simulationApp.fps.fps);
}

TimeLabel::TimeLabel() { w = 200; fixed_w = true; }
void TimeLabel::Sync(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	txt = simulationApp.getLocalizedString(L"lb_time") + FormatFixed2(simulationApp.t);
}
MousePositionLabel::MousePositionLabel() { w = 300; fixed_w = true; }
void MousePositionLabel::Sync(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	txt = simulationApp.getLocalizedString(L"lb_mouse") + ToWString(simulationApp.input.msp);
}

BottomPanel::BottomPanel(App& app) : Panel(Panel::bottom(app)) {
	vector<Control*> tmp;
	lb_fps = std::make_shared<FrameRateLabel>(); lb_time = std::make_shared<TimeLabel>(); lb_mouse = std::make_shared<MousePositionLabel>();
	tmp = { &*lb_fps, &*lb_time, &*lb_mouse };
	clx = std::make_shared<HorizontalControlList>(tmp);
	c = &*clx;
	Init(app);
}



