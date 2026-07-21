#include "CommandPanel.h"
#include "SimulationApp.h"
#include "Value.h"

RTbCmd::RTbCmd() : RichTextBox(560, 500) {}
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
	rtb = std::make_shared<RTbCmd>();
	bt_compile = std::make_shared<BtCompile>();
	tmp = { &*rtb, &*bt_compile };
	cly = std::make_shared<CtrlListY>(tmp);
	c = &*cly;
	Init(app);
}



