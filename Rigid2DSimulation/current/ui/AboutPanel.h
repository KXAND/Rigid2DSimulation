#pragma once
#include "ControlList.h"
#include "RichLabel.h"
#include "FloatPanel.h"

struct AboutPanel : LocalizedFloatPanel {
	std::shared_ptr<LocalizedRichLabel> aboutText;
	std::shared_ptr<VerticalControlList> controls;
	AboutPanel(App& app);
};

