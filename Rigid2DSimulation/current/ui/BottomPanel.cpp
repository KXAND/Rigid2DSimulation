#include "BottomPanel.h"
#include "SimulationApp.h"

FrameRateLabel::FrameRateLabel() { w = 450; fixedWidth = true; }
void FrameRateLabel::Sync(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	text = simulationApp.getLocalizedString(L"frm_tm") + FormatFixed2(simulationApp.fps.frm_time * 1000) +
		L", FPS: " + FormatFixed2(simulationApp.fps.fps);
}

TimeLabel::TimeLabel() { w = 200; fixedWidth = true; }
void TimeLabel::Sync(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	text = simulationApp.getLocalizedString(L"timeLabel") + FormatFixed2(simulationApp.simulationTime);
}
MousePositionLabel::MousePositionLabel() { w = 300; fixedWidth = true; }
void MousePositionLabel::Sync(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	text = simulationApp.getLocalizedString(L"mousePositionLabel") + ToWString(simulationApp.input.msp);
}

BottomPanel::BottomPanel(App& app) : Panel(Panel::CreateBottomPanel(app)) {
	vector<Control*> tmp;
	frameRateLabel = std::make_shared<FrameRateLabel>(); timeLabel = std::make_shared<TimeLabel>(); mousePositionLabel = std::make_shared<MousePositionLabel>();
	tmp = { &*frameRateLabel, &*timeLabel, &*mousePositionLabel };
	controls = std::make_shared<HorizontalControlList>(tmp);
	contentControl = &*controls;
	Init(app);
}



