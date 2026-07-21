#include "AboutPanel.h"
#include "SimulationApp.h"


FPnAbout::FPnAbout(App& app) : LocalizedFloatPanel(app, 600, 540, L"fpn_about") {
	vector<Control*> tmp;
	rtb_about = std::make_shared<LocalizedRichLabel>(560, L"about_txt");
	tmp = { &*rtb_about };
	cly = std::make_shared<VerticalControlList>(tmp);
	c = &*cly;
	Init(app);
}



