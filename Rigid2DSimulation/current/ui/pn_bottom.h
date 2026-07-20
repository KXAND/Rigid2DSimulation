#pragma once
#include "panel.h"
#include "label.h"
#include "ctrl_list.h"

struct LbFPS : Label {
	LbFPS();
	void Sync(App& app) override;
};

struct LbTime : Label {
	LbTime();
	void Sync(App& app) override;
};
struct LbMouse : Label {
	LbMouse();
	void Sync(App& app) override;
};

struct PnBottom : Panel {
	std::shared_ptr<LbFPS> lb_fps;
	std::shared_ptr<LbTime> lb_time;
	std::shared_ptr<LbMouse> lb_mouse;
	std::shared_ptr<CtrlListX> clx;
	PnBottom(App& app);
};

