#pragma once
#include "button.h"
#include "ctrl_list.h"
#include "float_panel.h"
#include "rich_textbox.h"

struct RTbCmd : RichTextbox {
	RTbCmd();
	void Sync(App& app) override;
	void OnDone(App& app) const override;
};
struct BtCompile : BtLan {
	BtCompile();
	void OnClick(App& app) override;
};

struct FPnCmd : FPnLan {
	std::shared_ptr<RTbCmd> rtb;
	std::shared_ptr<BtCompile> bt_compile;
	std::shared_ptr<CtrlListY> cly;
	FPnCmd(App& app);
};

