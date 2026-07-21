#pragma once
#include "Button.h"
#include "ControlList.h"
#include "FloatPanel.h"
#include "RichTextBox.h"

struct CommandScriptEditor : RichTextBox {
	CommandScriptEditor();
	void Sync(App& app) override;
	void OnDone(App& app) const override;
};
struct CompileButton : LocalizedButton {
	CompileButton();
	void OnClick(App& app) override;
};

struct CommandPanel : LocalizedFloatPanel {
	std::shared_ptr<CommandScriptEditor> rtb;
	std::shared_ptr<CompileButton> bt_compile;
	std::shared_ptr<VerticalControlList> cly;
	CommandPanel(App& app);
};

