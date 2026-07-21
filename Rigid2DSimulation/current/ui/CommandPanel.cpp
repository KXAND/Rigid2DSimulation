#include "CommandPanel.h"
#include "SimulationApp.h"
#include "Value.h"

CommandScriptEditor::CommandScriptEditor() : RichTextBox(560, 500) {}
void CommandScriptEditor::Sync(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); str = simulationApp.temporaryScriptText; }
void CommandScriptEditor::OnDone(App& app) const { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.temporaryScriptText = str; }

CompileButton::CompileButton() : LocalizedButton(200, L"compile") {}
void CompileButton::OnClick(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	simulationApp.scriptText = simulationApp.temporaryScriptText;
	Execute(simulationApp.currentScope, Compile(simulationApp.scriptText));
}

CommandPanel::CommandPanel(App& app) : LocalizedFloatPanel(app, 600, 600, L"fpn_cmd") {
	vector<Control*> tmp;
	scriptEditor = std::make_shared<CommandScriptEditor>();
	compileButton = std::make_shared<CompileButton>();
	tmp = { &*scriptEditor, &*compileButton };
	controls = std::make_shared<VerticalControlList>(tmp);
	c = &*controls;
	Init(app);
}



