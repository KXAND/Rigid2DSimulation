#pragma once
#include "ControlList.h"
#include "RichLabel.h"
#include "FloatPanel.h"

struct FPnAbout : LocalizedFloatPanel {
	std::shared_ptr<LocalizedRichLabel> rtb_about;
	std::shared_ptr<VerticalControlList> cly;
	FPnAbout(App& app);
};

