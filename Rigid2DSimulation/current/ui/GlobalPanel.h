#pragma once
#include "Label.h"
#include "Button.h"
#include "TextBox.h"
#include "ControlList.h"
#include "FloatPanel.h"
#include "RichLabel.h"
#include "Separator.h"

struct BtSave : BtLan {
	BtSave();
	void OnClick(App& app) override;
};
struct BtLoad : BtLan {
	BtLoad();
	void OnClick(App& app) override;
};
struct RLbGlobal : RichLabel {
	void Sync(App& app) override;
};

struct FPnGlobal : FPnLan {
	std::shared_ptr<Textbox> tb_save;
	std::shared_ptr<BtSave> bt_save;
	std::shared_ptr<CtrlListX> clx_save;
	std::shared_ptr<Textbox> tb_load;
	std::shared_ptr<BtLoad> bt_load;
	std::shared_ptr<CtrlListX> clx_load;
	std::shared_ptr<SeparatorY> spy;
	std::shared_ptr<RLbGlobal> rlb_global;
	std::shared_ptr<CtrlListY> cly;
	FPnGlobal(App& app);
};

