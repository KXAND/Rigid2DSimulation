#include "fpn_cmd.h"
#include "simulation_app.h"
#include "var.h"

RTbCmd::RTbCmd() : RichTextbox(560, 500) {}
void RTbCmd::Sync(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); str = simulationApp.temporaryScriptText; }
void RTbCmd::OnDone(App& app) const { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.temporaryScriptText = str; }

BtCompile::BtCompile() : BtLan(200, L"compile") {}
void BtCompile::OnClick(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	simulationApp.scriptText = simulationApp.temporaryScriptText;
	Execute(simulationApp.currentScope, Compile(simulationApp.scriptText));
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



