#pragma once
#include "button.h"
#include "ctrl_list.h"
#include "float_panel.h"

struct BtChi : Button {
	BtChi();
	void OnClick(App& app) override;
};
struct BtEng : Button {
	BtEng();
	void OnClick(App& app) override;
};

struct FPnLang : FPnLan {
	std::shared_ptr<BtChi> bt_chi;
	std::shared_ptr<BtEng> bt_eng;
	std::shared_ptr<CtrlListY> cly;
	FPnLang(App& app);
};

