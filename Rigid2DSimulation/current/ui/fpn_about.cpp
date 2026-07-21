#include "fpn_about.h"
#include "SimulationApp.h"


FPnAbout::FPnAbout(App& app) : FPnLan(app, 600, 540, L"fpn_about") {
	vector<Control*> tmp;
	rtb_about = std::make_shared<RLbLan>(560, L"about_txt");
	tmp = { &*rtb_about };
	cly = std::make_shared<CtrlListY>(tmp);
	c = &*cly;
	Init(app);
}



