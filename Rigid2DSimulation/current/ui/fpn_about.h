#pragma once
#include "ctrl_list.h"
#include "rich_label.h"
#include "float_panel.h"

struct FPnAbout : FPnLan {
	std::shared_ptr<RLbLan> rtb_about;
	std::shared_ptr<CtrlListY> cly;
	FPnAbout(App& app);
};

