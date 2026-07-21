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
	std::shared_ptr<FrameRateLabel> lb_fps;
	std::shared_ptr<TimeLabel> lb_time;
	std::shared_ptr<MousePositionLabel> lb_mouse;
	std::shared_ptr<HorizontalControlList> clx;
	BottomPanel(App& app);
};

