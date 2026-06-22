#include "fpn_about.h"
#include "cur.h"

#define cur (*(Cur*)&app)

FPnAbout::FPnAbout(App& app) : FPnLan(app, 600, 540, L"fpn_about") {
	vector<Control*> tmp;
	mkp(rtb_about)(560, L"about_txt");
	tmp = { &*rtb_about };
	mkp(cly)(tmp);
	c = &*cly;
	Init(app);
}


