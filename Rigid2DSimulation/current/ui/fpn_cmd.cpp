#include "fpn_cmd.h"
#include "simulation_app.h"
#include "var.h"

#define cur (*(SimulationApp*)&app)

RTbCmd::RTbCmd() : RichTextbox(560, 500) {}
void RTbCmd::Sync(App& app) { str = cur.temporaryScriptText; }
void RTbCmd::OnDone(App& app) const { cur.temporaryScriptText = str; }

BtCompile::BtCompile() : BtLan(200, L"compile") {}
void BtCompile::OnClick(App& app) {
	cur.scriptText = cur.temporaryScriptText;
	Execute(cur.curScope, Compile(cur.scriptText));
}

FPnCmd::FPnCmd(App& app) : FPnLan(app, 600, 600, L"fpn_cmd") {
	vector<Control*> tmp;
	mkp(rtb)();
	mkp(bt_compile)();
	tmp = { &*rtb, &*bt_compile };
	mkp(cly)(tmp);
	c = &*cly;
	Init(app);
}



