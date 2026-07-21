#include "AboutPanel.h"
#include "SimulationApp.h"


AboutPanel::AboutPanel(App& app) : LocalizedFloatPanel(app, 600, 540, L"fpn_about") {
	vector<Control*> tmp;
	aboutText = std::make_shared<LocalizedRichLabel>(560, L"about_txt");
	tmp = { &*aboutText };
	controls = std::make_shared<VerticalControlList>(tmp);
	c = &*controls;
	Init(app);
}



