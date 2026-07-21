#include "AboutPanel.h"
#include "SimulationApp.h"


AboutPanel::AboutPanel(App& app) : LocalizedFloatPanel(app, 600, 540, L"fpn_about") {
	vector<Control*> controls;
	aboutText = std::make_shared<LocalizedRichLabel>(560, L"about_txt");
	controls = { &*aboutText };
	this->controls = std::make_shared<VerticalControlList>(controls);
	contentControl = &*this->controls;
	Init(app);
}



