#pragma once
#include "Panel.h"
#include "Label.h"
#include "ControlList.h"

struct FrameRateLabel : Label {
	FrameRateLabel();
	void Sync(App& app) override;
};

struct TimeLabel : Label {
	TimeLabel();
	void Sync(App& app) override;
};
struct MousePositionLabel : Label {
	MousePositionLabel();
	void Sync(App& app) override;
};

struct BottomPanel : Panel {
	std::shared_ptr<FrameRateLabel> frameRateLabel;
	std::shared_ptr<TimeLabel> timeLabel;
	std::shared_ptr<MousePositionLabel> mousePositionLabel;
	std::shared_ptr<HorizontalControlList> controls;
	BottomPanel(App& app);
};

