#pragma once
#include "Label.h"
#include "Button.h"
#include "TextBox.h"
#include "ControlList.h"
#include "FloatPanel.h"
#include "RichLabel.h"
#include "Separator.h"

struct BtSave : LocalizedButton {
	BtSave();
	void OnClick(App& app) override;
};
struct BtLoad : LocalizedButton {
	BtLoad();
	void OnClick(App& app) override;
};
struct RLbGlobal : RichLabel {
	void Sync(App& app) override;
};

struct FPnGlobal : LocalizedFloatPanel {
	std::shared_ptr<TextBox> tb_save;
	std::shared_ptr<BtSave> bt_save;
	std::shared_ptr<HorizontalControlList> clx_save;
	std::shared_ptr<TextBox> tb_load;
	std::shared_ptr<BtLoad> bt_load;
	std::shared_ptr<HorizontalControlList> clx_load;
	std::shared_ptr<HorizontalSeparator> spy;
	std::shared_ptr<RLbGlobal> rlb_global;
	std::shared_ptr<VerticalControlList> cly;
	FPnGlobal(App& app);
};

