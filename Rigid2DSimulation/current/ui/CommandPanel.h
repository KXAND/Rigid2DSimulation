#pragma once
#include "Button.h"
#include "ControlList.h"
#include "FloatPanel.h"
#include "RichTextBox.h"

struct RTbCmd : RichTextBox {
	RTbCmd();
	void Sync(App& app) override;
	void OnDone(App& app) const override;
};
struct BtCompile : LocalizedButton {
	BtCompile();
	void OnClick(App& app) override;
};

struct FPnCmd : LocalizedFloatPanel {
	std::shared_ptr<RTbCmd> rtb;
	std::shared_ptr<BtCompile> bt_compile;
	std::shared_ptr<VerticalControlList> cly;
	FPnCmd(App& app);
};

