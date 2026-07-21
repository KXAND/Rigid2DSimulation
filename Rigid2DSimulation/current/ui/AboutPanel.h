#pragma once
#include "ControlList.h"
#include "RichLabel.h"
#include "FloatPanel.h"

struct AboutPanel : LocalizedFloatPanel {
	std::shared_ptr<LocalizedRichLabel> rtb_about;
	std::shared_ptr<VerticalControlList> cly;
	AboutPanel(App& app);
};

