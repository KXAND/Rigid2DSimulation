#pragma once
#include "ControlList.h"
#include "RichLabel.h"
#include "FloatPanel.h"

struct FPnAbout : FPnLan {
	std::shared_ptr<RLbLan> rtb_about;
	std::shared_ptr<CtrlListY> cly;
	FPnAbout(App& app);
};

