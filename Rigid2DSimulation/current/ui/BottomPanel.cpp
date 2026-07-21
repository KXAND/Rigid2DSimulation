#include "BottomPanel.h"
#include "SimulationApp.h"

FrameRateLabel::FrameRateLabel() { w = 450; fixedWidth = true; }
void FrameRateLabel::Sync(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	text = simulationApp.GetLocalizedString(L"frm_tm") + FormatFixed2(simulationApp.fps.averageFrameTime * 1000) +
		L", FPS: " + FormatFixed2(simulationApp.fps.framesPerSecond);
}

TimeLabel::TimeLabel() { w = 200; fixedWidth = true; }
void TimeLabel::Sync(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	text = simulationApp.GetLocalizedString(L"timeLabel") + FormatFixed2(simulationApp.simulationTime);
}
MousePositionLabel::MousePositionLabel() { w = 300; fixedWidth = true; }
void MousePositionLabel::Sync(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	text = simulationApp.GetLocalizedString(L"mousePositionLabel") + ToWString(simulationApp.input.mousePosition);
}

BottomPanel::BottomPanel(App& app) : Panel(Panel::CreateBottomPanel(app)) {
	vector<Control*> tmp;
	frameRateLabel = std::make_shared<FrameRateLabel>(); timeLabel = std::make_shared<TimeLabel>(); mousePositionLabel = std::make_shared<MousePositionLabel>();
	tmp = { &*frameRateLabel, &*timeLabel, &*mousePositionLabel };
	controls = std::make_shared<HorizontalControlList>(tmp);
	contentControl = &*controls;
	Init(app);
}



